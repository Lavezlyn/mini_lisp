#include "./forms.h"
#include "./error.h"
#include "./builtin.h"

using namespace std::literals;

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define"s, defineForm},
    {"set!"s, defineForm},
    {"quote"s, quoteForm},
    {"quasiquote"s,quasiquoteForm},
    {"if"s, ifForm},
    {"and"s, andForm},
    {"or"s, orForm},
    {"lambda"s, lambdaForm},
    {"cond"s, condForm},
    {"case"s, caseForm},
    {"let"s, letForm},
    {"let*"s, letStarForm},
    {"begin"s, beginForm},
    {"delay"s, delayForm},
    {"force"s, forceForm},
    {"do"s, doForm}
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

ValuePtr caseForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if(args.size() < 2) throw LispError("Incorrect number of arguments for 'case'");
    auto key = e.eval(args[0]);
    for(size_t i = 1; i < args.size(); i++){
        if(args[i]->getType() != ValueType::PAIR) throw LispError("Invalid case form, malformed clause");
        auto Pair = static_cast<PairValue*>(args[i].get());
        std::vector<ValuePtr> PairVec = Pair->toVector();
        auto datum = PairVec[0];
        if(datum->asSymbol() && *datum->asSymbol() == "else"s){
            if(i == args.size()-1){
                ValuePtr result = nullptr;
                for(size_t j = 1; j < PairVec.size(); j++)
                    result = e.eval(PairVec[j]);
                return result;
            }
            else throw LispError("Else clause not last in case form");
        }
        if(datum->getType() != ValueType::PAIR) throw LispError("Invalid case form, malformed clause");
        auto PairVec2 = static_cast<PairValue*>(datum.get())->toVector();
        if(PairVec2.size() != 1) throw LispError("Invalid case form, incorrect number of literals");
        auto literal = PairVec2[0];
        if(!literal->isAtom()) throw LispError("Invalid case form, datum not a literal");
        if(static_cast<BooleanValue*>(equal({key, literal}, e).get())->getValue()){
            ValuePtr result = nullptr;
            for(size_t j = 1; j < PairVec.size(); j++)
                result = e.eval(PairVec[j]);
            return result;
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

ValuePtr letStarForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if (args.size() < 2){
        throw LispError("Incorrect number of arguments");
    }
    std::vector<ValuePtr> vars;    
    std::vector<ValuePtr> vals;
    std::vector<ValuePtr> definitions = args[0]->toVector();    
    auto child = e.shared_from_this();
    for (const auto& i: definitions){
        if(i->getType() != ValueType::PAIR){
            throw LispError("Invalid definition");
        }
        auto Pair = static_cast<PairValue*>(i.get());
        std::vector<ValuePtr> def = Pair->toVector();   
        if(def.size() != 2) throw LispError("Invalid definition");
        vars.push_back(def[0]);
        vals.push_back(child->eval(def[1]));
        child = child->createChild({*def[0]->asSymbol()}, {vals.back()});
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    auto lambda = std::make_shared<LambdaValue>(vars, body, child);
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

ValuePtr delayForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if(args.size() != 1)
        throw LispError("Incorrect number of arguments for 'delay'");
    return make_shared<PromiseValue>(args[0], e.shared_from_this());
}

ValuePtr forceForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if(args.size() != 1)
        throw LispError("Incorrect number of arguments for 'force'");
    auto val = e.eval(args[0]);
    if(val->getType() != ValueType::PROMISE)
        throw LispError("Invalid argument for 'force'");
    return static_cast<PromiseValue*>(val.get())->force();
}

ValuePtr doForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    if(args.size() < 2)
        throw LispError("Incorrect number of arguments for 'do'");
    auto varList = static_cast<PairValue*>(args[0].get())->toVector();
    std::vector<std::string> names; std::vector<ValuePtr> inits; std::vector<ValuePtr> steps;
    for(const auto& i: varList){
        if(i->getType() != ValueType::PAIR)
            throw LispError("Invalid variable list");
        auto wholeVar = static_cast<PairValue*>(i.get())->toVector();
        if(wholeVar.size() != 3)
            throw LispError("Invalid variable list");
        if(wholeVar[0]->getType() != ValueType::SYMBOL)
            throw LispError("Invalid variable name");
        names.push_back(*wholeVar[0]->asSymbol());
        inits.push_back(wholeVar[1]);
        steps.push_back(wholeVar[2]);
    }
    if(args[1]->getType() != ValueType::PAIR)
        throw LispError("Invalid test clause in do form");
    auto test = static_cast<PairValue*>(args[1].get())->toVector();
    auto body = std::vector<ValuePtr>(args.begin() + 2, args.end());
    auto child = e.createChild(names, inits);
    while(true){
        auto testResult = child->eval(test[0]);
        if(testResult->getType() != ValueType::BOOLEAN)
            throw LispError("Invalid test in do form");
        if(static_cast<BooleanValue*>(testResult.get())->getValue()){
            ValuePtr result = std::make_shared<NilValue>();
            for(size_t i = 1; i < test.size(); i++)
                result = child->eval(test[i]);
            return result;
        }
        for(const auto& i: body)
            child->eval(i);
        for(size_t i = 0; i < names.size(); i++)
            child->defineBinding(names[i], child->eval(steps[i]));  
    }
}