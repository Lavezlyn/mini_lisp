#include "./eval_env.h"
#include "./error.h"
#include "./builtin.h"
#include <algorithm>
#include <iterator>

using namespace std::literals;

EvalEnv::EvalEnv(){
    env["+"] = std::make_shared<BuiltinProcValue>(add);
    env["-"] = std::make_shared<BuiltinProcValue>(sub);
    env["*"] = std::make_shared<BuiltinProcValue>(mul);
    env["/"] = std::make_shared<BuiltinProcValue>(divide);
    env["print"] = std::make_shared<BuiltinProcValue>(print);
}

ValuePtr EvalEnv::eval(ValuePtr expr){
    if (expr->isSelfEvaluating()){
        return expr;
    }
    if (expr->getType() == ValueType::PAIR){
        std::vector<ValuePtr> list = static_cast<PairValue*>(expr.get())->toVector();
        if (list[0]->asSymbol() == "define"s){
            if(auto name = list[1]->asSymbol()){
                env[*name] = eval(list[2]);
                return std::make_shared<NilValue>(); 
            }
            else{
                throw LispError("Malformed define.");
            }
        }
        else{
            ValuePtr proc = this->eval(list[0]);
            ValuePtr rest = static_cast<PairValue*>(expr.get())->getCdr();
            std::vector<ValuePtr> args = this->evalList(rest);
            return this->apply(proc, args);
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