#include "./eval_env.h"
#include "./error.h"
#include "./builtin.h"
#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <unordered_set>

using namespace std::literals;
std::unordered_set<std::string> special_forms = {"define"s};
std::unordered_set<std::string> calculator = {"+"s, "-"s, "*"s, "/"s, "print"s};
std::unordered_set<std::string> typeCheck = { "atom?"s, "boolean?"s, "number?"s, 
                                            "pair?"s, "procedure?"s, "symbol?"s, 
                                            "null?"s,"string?"s, "integer?"s, "list?"s };

EvalEnv::EvalEnv(){
    env["+"] = std::make_shared<BuiltinProcValue>(add);
    env["-"] = std::make_shared<BuiltinProcValue>(sub);
    env["*"] = std::make_shared<BuiltinProcValue>(mul);
    env["/"] = std::make_shared<BuiltinProcValue>(divide);
    env["print"] = std::make_shared<BuiltinProcValue>(print);
}

void EvalEnv::defineBinding(const std::string& name, ValuePtr value){
    env[name] = value;
}

ValuePtr EvalEnv::lookupBinding(const std::string& name){
    if (auto value = env[name]){
        return value;
    }
    else if (parent){
        return parent->lookupBinding(name);
    }
    else{
        throw LispError("Variable " + name + " not defined.");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr){
    if (expr->isSelfEvaluating()){
        return expr;
    }
    if (expr->getType() == ValueType::PAIR){
        auto Pair = static_cast<PairValue*>(expr.get());
        auto op = Pair->getCar()->asSymbol();
        ValuePtr rest = Pair->getCdr();
        std::vector<ValuePtr> args = rest->toVector();
        if (SPECIAL_FORMS.find(*op) != SPECIAL_FORMS.end()){
            return SPECIAL_FORMS.at(*op)(args, *this);   
        }
        else if (typeCheck.find(*op) != typeCheck.end()){
            auto checker = static_cast<BuiltinProcValue*>(env[*op].get())->getFunc();
            std::vector<ValuePtr> evaled_args = this->evalList(rest);
            return checker(evaled_args);
        }
        else if (calculator.find(*op)!=calculator.end()){
            ValuePtr proc = this->eval(Pair->getCar());
            std::vector<ValuePtr> evaled_args = this->evalList(rest);
            return this->apply(proc, evaled_args);
        }
    }
    if (auto symbol = expr->asSymbol()){
        if (auto value = env[*symbol]){
            return value;
        }
        else{
            throw LispError("Variable " + *symbol + " not defined.");
        }
    }
    else if (expr->isNil()){
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
    else {
        throw LispError("Unimplemented");
    }
}