#include "./forms.h"

using namespace std::literals;
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define"s, defineForm},
    {"quote"s, quoteForm},
    {"if"s, ifForm},
    {"and"s, andForm},
    {"or"s, orForm}
};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (auto name = args[0]->asSymbol()) {
        e.defineBinding(*name, e.eval(args[1]));
        return std::make_shared<NilValue>();
    } 
    else if (args[0]->getType() == ValueType::PAIR){
        auto Pair = static_cast<PairValue*>(args[0].get());
        auto name = static_cast<SymbolValue*>(Pair->getCar().get())->getValue();
        auto params = Pair->getCdr()->toVector();
        auto body = args[1]->toVector();
        e.defineBinding(name, make_shared<LambdaValue>(params, body));
        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("Unimplemented");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (args.size() != 1) {
        throw LispError("Too many operands in the form");
    }
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (args.size() != 3) {
        throw LispError("Too many operands in the form");
    }
    auto cond = e.eval(args[0]);
    if (cond->isBoolean()){
        if(!static_cast<BooleanValue*>(cond.get())->getValue()){
            return e.eval(args[2]);
        }
    }
    return e.eval(args[1]);
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if (args.size() == 0){
        return std::make_shared<BooleanValue>(true); 
    }
    for (const auto& i: args){
        if (auto val = e.eval(i); val->isBoolean()){
            if (!static_cast<BooleanValue*>(val.get())->getValue()){
                return std::make_shared<BooleanValue>(false);
            }
        }
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if (args.size() == 0){
        return std::make_shared<BooleanValue>(false); 
    }
    for (const auto& i: args){
        auto val = e.eval(i);
        if (val->isBoolean()){
            if (!static_cast<BooleanValue*>(val.get())->getValue()){
                continue;
            }
        }
        return val;
    }
    return e.eval(args[args.size()-1]);
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if (args.size() != 2){
        throw LispError("Too many operands in the form");
    }
    if (args[0]->getType() != ValueType::PAIR){
        throw LispError("Invalid lambda form");
    }
    auto params = args[0]->toVector();
    auto body = args[1]->toVector();
    return std::make_shared<LambdaValue>(params, body);
}