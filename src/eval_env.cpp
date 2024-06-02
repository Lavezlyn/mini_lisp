#include "./eval_env.h"
#include "./error.h"
#include "./builtin.h"
#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <unordered_set>

using namespace std::literals;

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent) : parent{parent} {
    for(auto& [name, proc] : BUILTIN)
        env[name] = proc;
}

std::shared_ptr<EvalEnv> EvalEnv::createGlobal(){
        return std::shared_ptr<EvalEnv>(new EvalEnv(nullptr));
}; 

std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto child = std::shared_ptr<EvalEnv>(new EvalEnv(shared_from_this()));
    for (size_t i = 0; i < params.size(); i++)
        child->defineBinding(params[i], args[i]);
    return child;
}

void EvalEnv::defineBinding(const std::string& name, ValuePtr value){
    env[name] = value;
}

ValuePtr EvalEnv::lookupBinding(const std::string& name){
    if(this->env.find(name) == this->env.end() && this->parent)
        return this->parent->lookupBinding(name);
    else if(this->env.find(name) != this->env.end())
        return this->env[name];
    else
        throw LispError("Variable " + name + " not defined.");
}

ValuePtr EvalEnv::eval(ValuePtr expr){
    if (expr->isSelfEvaluating())
        return expr;
    else if (auto symbol = expr->asSymbol()){
        if (auto value = lookupBinding(*symbol))
            return value;
        else
            throw LispError("Variable " + *symbol + " not defined.");
    }
    else if (expr->getType() == ValueType::PAIR){
        auto islist = list({expr}, *this);
        if(!static_cast<BooleanValue*>(islist.get())->getValue()){
            std::string msg = "Malformed list: " + expr->toString();
            throw LispError("Malformed list");
        }
        else{
            auto Pair = static_cast<PairValue*>(expr.get());
            auto op = Pair->getCar()->asSymbol();
            if (op.has_value() && SPECIAL_FORMS.find(*op) != SPECIAL_FORMS.end())
                return SPECIAL_FORMS.at(*op)(Pair->getCdr()->toVector(), *this);   
            else
            {
                ValuePtr operands = Pair->getCdr();
                auto proc = this->eval(Pair->getCar());
                if(proc->getType()==ValueType::BUILTIN_PROC || proc->getType()==ValueType::LAMBDA){
                    std::vector<ValuePtr> args = evalList(operands);
                    return this->apply(proc, args);
                }
                else return expr;
            }
        }
    }
    else if (expr->isNil()){
        throw LispError("Cannot evaluate nil");
    }
    else {
        throw LispError("Undefined expression type");
    }
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(static_cast<PairValue*>(expr.get())->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        auto procedure = static_cast<BuiltinProcValue*>(proc.get())->getFunc();
        return procedure(args, *this);
    } 
    else if (typeid(*proc) == typeid(LambdaValue)) {
        auto lambda = static_cast<LambdaValue*>(proc.get());
        return lambda->apply(args);
    } 
    else throw LispError("Invalid procedure type");
}