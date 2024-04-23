#include "./eval_env.h"
#include "./error.h"
#include "./builtin.h"
#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <iostream>

using namespace std::literals;
std::unordered_set<std::string> special_forms = {"define"s};
std::unordered_set<std::string> calculator = {"+"s, "-"s, "*"s, "/"s, "print"s};
std::unordered_set<std::string> typeCheck = { "atom?"s, "boolean?"s, "number?"s, 
                                            "pair?"s, "procedure?"s, "symbol?"s, 
                                            "null?"s,"string?"s, "integer?"s, "list?"s };

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent) : parent{parent} {
    for(auto& [name, proc] : BUILTIN){
        env[name] = proc;
    }
}

std::shared_ptr<EvalEnv> EvalEnv::createGlobal(){
        return std::shared_ptr<EvalEnv>(new EvalEnv(nullptr));
    }; 
std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto child = std::shared_ptr<EvalEnv>(new EvalEnv(shared_from_this()));
    for (size_t i = 0; i < params.size(); i++) {
        child->defineBinding(params[i], args[i]);
    }
    return child;
}

void EvalEnv::defineBinding(const std::string& name, ValuePtr value){
    env[name] = value;
}

ValuePtr EvalEnv::lookupBinding(const std::string& name){
    if(env.find(name) == env.end() && parent){
        return parent->lookupBinding(name);
    }
    else if(env.find(name) != env.end()){
        return env[name];
    }
    else{
        throw LispError("Variable " + name + " not defined.");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr){
    if (expr->isSelfEvaluating()){
        return expr;
    }
    if (auto symbol = expr->asSymbol()){
        if (auto value = lookupBinding(*symbol)){
            return eval(value);
        }
        else{
            throw LispError("Variable " + *symbol + " not defined.");
        }
    }
    if (expr->getType() == ValueType::PAIR){
        auto Pair = static_cast<PairValue*>(expr.get());
        auto op = Pair->getCar()->asSymbol();
        if (SPECIAL_FORMS.find(*op) != SPECIAL_FORMS.end()){
            return SPECIAL_FORMS.at(*op)(Pair->getCdr()->toVector(), *this);   
        }
        else{
            ValuePtr operands = Pair->getCdr();
            std::vector<ValuePtr> args = evalList(operands);
            auto proc = this->eval(Pair->getCar());
            return this->apply(proc, args);
        }
    }
    if (expr->isNil()){
        throw LispError("Evaluating nil is prohibited.");
    }
    else {
        throw LispError("Unimplemented.");
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
        return procedure(args);
    } 
    if (typeid(*proc) == typeid(LambdaValue)) {
        auto lambda = static_cast<LambdaValue*>(proc.get());
        return lambda->apply(args);
    } 
    else {
        throw LispError("Unimplemented");
    }
}