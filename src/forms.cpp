#include "./forms.h"
#include "./error.h"
#include "./builtin.h"

using namespace std::literals;

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define"s, defineForm},
    {"quote"s, quoteForm},
    {"quasiquote"s,quasiquoteForm},
    {"if"s, ifForm},
    {"and"s, andForm},
    {"or"s, orForm},
    {"lambda"s, lambdaForm},
    {"cond"s, condForm},
    {"let"s, letForm},
    {"begin"s, beginForm}
};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& e) {
    if (auto name = args[0]->asSymbol()) {
        if (args[1]->getType() == ValueType::SYMBOL){
            e.defineBinding(*name, args[1]);
        }
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
    if (args.size() != 3 && args.size() != 2) {
        throw LispError("Incorrest number of arguments");
    }
    auto cond = e.eval(args[0]);
    if(args.size() == 3){
        if (cond->isBoolean()){
            if(!static_cast<BooleanValue*>(cond.get())->getValue())
                return e.eval(args[2]);
        }
        return e.eval(args[1]);
    }
    else {
        if (cond->isBoolean()){
            if(static_cast<BooleanValue*>(cond.get())->getValue())
                return e.eval(args[1]);
        }
        else return std::make_shared<NilValue>();
    }
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

ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    for(size_t i = 0; i < args.size(); i++){
        auto Pair = static_cast<PairValue*>(args[i].get());
        std::vector<ValuePtr> PairVec = Pair->toVector();
        if (PairVec.size() == 1){return e.eval(PairVec[0]);}
        if(PairVec[0]->asSymbol() && *PairVec[0]->asSymbol() == "else"s){
            if(i == args.size() - 1)
                return e.eval(PairVec[1]);
            else throw LispError("Else clause not last in cond form");
        }
        auto cond = e.eval(PairVec[0]);
        if (cond->isBoolean() && !static_cast<BooleanValue*>(cond.get())->getValue()) continue;
        else {
            ValuePtr result = nullptr;
            return e.eval(PairVec[1]);  
        }
    } 
    return std::make_shared<NilValue>();
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if (args.size() < 2){
        throw LispError("Incorrect number of arguments");
    }
    std::vector<ValuePtr> vars;    
    std::vector<ValuePtr> vals;
    std::vector<ValuePtr> definitions = args[0]->toVector();    
    for (const auto& i: definitions){
        if(i->getType() != ValueType::PAIR){
            throw LispError("Invalid definition");
        }
        auto Pair = static_cast<PairValue*>(i.get());
        std::vector<ValuePtr> def = Pair->toVector();   
        if(def.size() != 2) throw LispError("Invalid definition");
        vars.push_back(def[0]);
        vals.push_back(e.eval(def[1]));
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    auto lambda = std::make_shared<LambdaValue>(vars, body, e.shared_from_this());
    return lambda.get()->apply(vals);   
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    ValuePtr result = nullptr;
    for (const auto& i: args){
        result = e.eval(i);
    }
    return result;
}

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    //FixMe:implement recursive quasiquote and unquote
    if(args.size() != 1)
        throw LispError("Incorrect number of arguments for 'quasiquote'");
    if(args[0]->getType() != ValueType::PAIR)
        return args[0];
    auto Pair = static_cast<PairValue*>(args[0].get());
    std::vector<ValuePtr> newArgs;
    for(const auto& arg : Pair->toVector()){
        if(arg->getType() == ValueType::PAIR){
            auto Pair2 = static_cast<PairValue*>(arg.get());
            std::vector<ValuePtr> PairVec = Pair2->toVector();
            if(PairVec.size() > 0 && PairVec[0]->asSymbol() && *PairVec[0]->asSymbol() == "unquote"s){
                if(PairVec.size() != 2)
                    throw LispError("Invalid unquote form");
                newArgs.push_back(e.eval(PairVec[1]));
            }
            else newArgs.push_back(arg);
        }
        else newArgs.push_back(arg);
    }
    return makelist(newArgs, e);
}