#include "./forms.h"
#include "./error.h"

using namespace std::literals;

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define"s, defineForm},
    {"quote"s, quoteForm},
    {"if"s, ifForm},
    {"and"s, andForm},
    {"or"s, orForm},
    {"lambda"s, lambdaForm}
};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (auto name = args[0]->asSymbol()) {
        e.defineBinding(*name, e.eval(args[1]));
        return std::make_shared<NilValue>();
    } 
    else if (args[0]->getType() == ValueType::PAIR){
        auto Pair = static_cast<PairValue*>(args[0].get());
        auto name = Pair->getCar()->asSymbol();
        if(!name){throw LispError("Invalid procedure name");}
        auto params = Pair->getCdr()->toVector();
        std::vector<ValuePtr> body;
        for (size_t i = 1; i < args.size(); i++){
            body.push_back(args[i]);
        }
        e.defineBinding(*name, make_shared<LambdaValue>(params, body, e.shared_from_this()));
        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("Malformed define form");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (args.size() != 1) {
        throw LispError("Incorrect number of arguments");
    }
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (args.size() != 3) {
        throw LispError("Incorrest number of arguments");
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
    return e.eval(args[args.size()-1]);
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
    auto params = args[0]->toVector();
    std::vector<ValuePtr> body;
    for (size_t i = 1; i < args.size(); i++){
        body.push_back(args[i]);
    }
    return make_shared<LambdaValue>(params, body, e.shared_from_this());
}