#include "./builtin.h"
#include "./error.h"
#include <iostream> 
#include <cmath>

class EvalEnv;

ValuePtr apply(const std::vector<ValuePtr>& params){
    auto proc = params[0];
    if(proc->getType() == ValueType::BUILTIN_PROC){
        auto func = static_cast<BuiltinProcValue*>(proc.get())->getFunc();
        return func(std::vector<ValuePtr>(params.begin() + 1, params.end()));
    }
    else if(proc->getType() == ValueType::LAMBDA){
        auto lambda = static_cast<LambdaValue*>(proc.get());
        return lambda->apply(std::vector<ValuePtr>(params.begin() + 1, params.end()));  
    } 
    else{
        throw LispError("Cannot apply a non-procedure.");
    }
}

ValuePtr display(const std::vector<ValuePtr>& params){
    for(const auto& i: params){
        if(i->getType() == ValueType::STRING)
            std::cout << static_cast<StringValue*>(i.get())->getValue();
        else std::cout << i->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr newline(const std::vector<ValuePtr>& params){
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr print(const std::vector<ValuePtr>& params){
    for(const auto& i: params){
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr displayln(const std::vector<ValuePtr>& params){
    auto result = display(params);
    auto result2 = newline(params);
    return std::make_shared<NilValue>();
}   

ValuePtr Error(const std::vector<ValuePtr>& params){
    if(params.size() > 1){
        throw LispError("Incorrect number of arguments.");
    }
    if(params.size() == 0){
        throw LispError("");
    }
    throw LispError(params[0]->toString());
}

ValuePtr Eval(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments.");
    }
    return params[0]; 
}

ValuePtr Exit(const std::vector<ValuePtr>& params){
    if (params.size() == 0)
        exit(0);
    else if (params.size() == 1){
        if(!params[0]->isNumber()){
            throw LispError("Cannot exit with a non-numeric value.");
        }
        exit(static_cast<NumericValue*>(params[0].get())->asNumber());
    }
    else throw LispError("Incorrect number of arguments.");
}


ValuePtr add(const std::vector<ValuePtr>& params){
    double result = 0;
    for(const auto& i: params){
        if(!i->isNumber()){
            throw LispError("Cannot add a non-numeric value.");
        }
        result += static_cast<NumericValue*>(i.get())->asNumber();
    }
    return std::make_shared<NumericValue>(result);  
}

ValuePtr sub(const std::vector<ValuePtr>& params){
    double result = 0;
    if (params.size() == 0) throw LispError("Incorrect number of arguments.");
    if (params.size() == 1) {
        if(!params[0]->isNumber()){
            throw LispError("Cannot substract a non-numeric value.");
        }
        result = -static_cast<NumericValue*>(params[0].get())->asNumber();
        return std::make_shared<NumericValue>(result);  
    }
    for(const auto& i: params){
        if(!i->isNumber()){
            throw LispError("Cannot substract a non-numeric value.");
        }
        result -= static_cast<NumericValue*>(i.get())->asNumber();
    }
    double first_elem = static_cast<NumericValue*>(params[0].get())->asNumber();
    result += 2*first_elem;
    return std::make_shared<NumericValue>(result);  
}

ValuePtr mul(const std::vector<ValuePtr>& params){
    double result = 1;
    for(const auto& i: params){
        if(!i->isNumber()){
            throw LispError("Cannot multiply a non-numeric value.");
        }
        result *= static_cast<NumericValue*>(i.get())->asNumber();
    }
    return std::make_shared<NumericValue>(result);  
}

ValuePtr divide(const std::vector<ValuePtr>& params){
    double result = 1;
    if (params.size() == 0) throw LispError("Incorrect number of arguments.");
    if (params.size() == 1){
        if(!params[0]->isNumber()){
            throw LispError("Cannot divide a non-numeric value.");
        }
        result = 1/static_cast<NumericValue*>(params[0].get())->asNumber();
        return std::make_shared<NumericValue>(result);  
    }
    for(const auto& i: params){
        if(!i->isNumber()){
            throw LispError("Cannot divide a non-numeric value.");
        }
        double temp = static_cast<NumericValue*>(i.get())->asNumber();
        if(temp == 0) throw LispError("Division by zero.");
        result /= temp;
    }
    double first_elem = static_cast<NumericValue*>(params[0].get())->asNumber();
    result = result * first_elem * first_elem;
    return std::make_shared<NumericValue>(result);  
}

ValuePtr ABS(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments.");
    }
    if(!params[0]->isNumber()){
        throw LispError("Cannot take the absolute value of a non-numeric value.");
    }
    double result = std::abs(static_cast<NumericValue*>(params[0].get())->asNumber());
    return std::make_shared<NumericValue>(result);  
}

ValuePtr expt(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double tmp1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double tmp2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    if(tmp1 == 0 || tmp2 == 0) throw LispError("Undefined mathematic form");
    double result = std::pow(tmp1, tmp2);
    return std::make_shared<NumericValue>(result);  
}

ValuePtr quotient(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double x = static_cast<NumericValue*>(params[0].get())->asNumber();
    double y = static_cast<NumericValue*>(params[1].get())->asNumber();
    if(y == 0) throw LispError("Division by zero");
    return std::make_shared<NumericValue>(static_cast<int>(x/y));  
}

ValuePtr modulo(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments.");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double x = static_cast<NumericValue*>(params[0].get())->asNumber();
    double y = static_cast<NumericValue*>(params[1].get())->asNumber();
    if(y == 0) throw LispError("Division by zero.");
    double result = x - y * std::floor(x / y);
    //TODO
    return std::make_shared<NumericValue>(result);
}

ValuePtr Remainder(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments.");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double x = static_cast<NumericValue*>(params[0].get())->asNumber();
    double y = static_cast<NumericValue*>(params[1].get())->asNumber();
    if(y == 0) throw LispError("Division by zero.");
    int quotient = static_cast<int>(x/y);
    return std::make_shared<NumericValue>(x - quotient*y);
}

ValuePtr eq(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments.");
    }
    if(params[0]->getType() != params[1]->getType())
        return std::make_shared<BooleanValue>(false);
    if(params[0]->isAtom() && params[0]->getType()!=ValueType::SYMBOL && params[0]->getType()!=ValueType::STRING)
        return std::make_shared<BooleanValue>(params[0]->toString() == params[1]->toString());
    else{
        return std::make_shared<BooleanValue>(params[0] == params[1]);
    }
}

ValuePtr equal(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments.");
    }
    if(params[0]->getType() != params[1]->getType())
        return std::make_shared<BooleanValue>(false);
    if(params[0]->isAtom() && params[0]->getType()!=ValueType::SYMBOL)
        return std::make_shared<BooleanValue>(params[0]->toString() == params[1]->toString());
    else{
        if(params[0]->getType() == ValueType::PAIR){
            auto list1 = static_cast<PairValue*>(params[0].get())->toVector();
            auto list2 = static_cast<PairValue*>(params[1].get())->toVector();
            if(list1.size() != list2.size()) return std::make_shared<BooleanValue>(false);
            for(size_t i = 0; i < list1.size(); i++){
                if(!equal({list1[i], list2[i]})->asBoolean()){
                    return std::make_shared<BooleanValue>(false);
                }
            }
            return std::make_shared<BooleanValue>(true);
        }
        else return std::make_shared<BooleanValue>(params[0] == params[1]);
    }
}

ValuePtr NOT(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments.");
    }
    if(params[0]->getType()==ValueType::BOOLEAN && !params[0]->asBoolean()){
        return std::make_shared<BooleanValue>(true);
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr numEq(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    return std::make_shared<BooleanValue>(static_cast<NumericValue*>(params[0].get())->asNumber() == static_cast<NumericValue*>(params[1].get())->asNumber());
}

ValuePtr odd(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(static_cast<int>(result) % 2 != 0);  
}

ValuePtr even(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(static_cast<int>(result) % 2 == 0);  
}

ValuePtr zero(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(result == 0);  
}

ValuePtr greater(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 > result2);  
}

ValuePtr less(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 < result2);  
}

ValuePtr lessEq(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 <= result2);  
}

ValuePtr greaterEq(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Incorrect number of arguments");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Non-numeric value");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 >= result2);  
}

ValuePtr boolean(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isBoolean());
}

ValuePtr atom(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isAtom());
}

ValuePtr null(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isNil());
}   

ValuePtr number(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isNumber());
}

ValuePtr pair(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::PAIR);
}

ValuePtr procedure(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::BUILTIN_PROC);
}

//TODO：determine before evaluation
ValuePtr symbol(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->asSymbol().has_value());
}

ValuePtr string(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::STRING);
}

ValuePtr integer(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isInteger());
}

//TODO: wrong performance
ValuePtr list(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Incorrect number of arguments");
    }
    if(params[0]->getType() != ValueType::PAIR){
        return std::make_shared<BooleanValue>(false);
    }
    auto list = static_cast<PairValue*>(params[0].get())->toVector();
    while(!list.empty()){
        if(list.back()->getType() != ValueType::PAIR && !list.back()->isNil()){
            return std::make_shared<BooleanValue>(false);
        }
        list.pop_back();
    }
    return std::make_shared<BooleanValue>(true);
}

extern std::unordered_map<std::string, ValuePtr> BUILTIN{
    {"apply", std::make_shared<BuiltinProcValue>(apply)},
    {"display", std::make_shared<BuiltinProcValue>(display)},
    {"displayln", std::make_shared<BuiltinProcValue>(displayln)},
    {"print", std::make_shared<BuiltinProcValue>(print)},
    {"newline", std::make_shared<BuiltinProcValue>(newline)},
    {"error", std::make_shared<BuiltinProcValue>(Error)},
    {"eval", std::make_shared<BuiltinProcValue>(Eval)},
    {"exit", std::make_shared<BuiltinProcValue>(Exit)},
    {"+", std::make_shared<BuiltinProcValue>(add)},
    {"-", std::make_shared<BuiltinProcValue>(sub)},
    {"*", std::make_shared<BuiltinProcValue>(mul)},
    {"/", std::make_shared<BuiltinProcValue>(divide)},
    {"abs", std::make_shared<BuiltinProcValue>(ABS)},
    {"quotient", std::make_shared<BuiltinProcValue>(quotient)},
    {"remainder", std::make_shared<BuiltinProcValue>(Remainder)},
    {"equal?", std::make_shared<BuiltinProcValue>(equal)},
    {"eq?", std::make_shared<BuiltinProcValue>(eq)},
    {"not", std::make_shared<BuiltinProcValue>(NOT)},
    {"odd?", std::make_shared<BuiltinProcValue>(odd)},
    {"even?", std::make_shared<BuiltinProcValue>(even)},
    {"zero?", std::make_shared<BuiltinProcValue>(zero)},
    {"<", std::make_shared<BuiltinProcValue>(less)},
    {">", std::make_shared<BuiltinProcValue>(greater)},
    {"=", std::make_shared<BuiltinProcValue>(numEq)},
    {"<=", std::make_shared<BuiltinProcValue>(lessEq)},
    {">=", std::make_shared<BuiltinProcValue>(greaterEq)},
    {"expt", std::make_shared<BuiltinProcValue>(expt)},
    {"modulo", std::make_shared<BuiltinProcValue>(modulo)},
    {"atom?", std::make_shared<BuiltinProcValue>(atom)},
    {"boolean?", std::make_shared<BuiltinProcValue>(boolean)},
    {"number?", std::make_shared<BuiltinProcValue>(number)},
    {"pair?", std::make_shared<BuiltinProcValue>(pair)},
    {"procedure?", std::make_shared<BuiltinProcValue>(procedure)},
    {"symbol?", std::make_shared<BuiltinProcValue>(symbol)},
    {"null?", std::make_shared<BuiltinProcValue>(null)},
    {"string?", std::make_shared<BuiltinProcValue>(string)},
    {"integer?", std::make_shared<BuiltinProcValue>(integer)},
    {"list?", std::make_shared<BuiltinProcValue>(list)}
};





