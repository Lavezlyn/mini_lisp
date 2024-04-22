#include "./builtin.h"
#include "./error.h"
#include <iostream> 

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
    for(const auto& i: params){
        if(!i->isNumber()){
            throw LispError("Cannot substract a non-numeric value.");
        }
        result /= static_cast<NumericValue*>(i.get())->asNumber();
    }
    double first_elem = static_cast<NumericValue*>(params[0].get())->asNumber();
    result = result * first_elem * first_elem;
    return std::make_shared<NumericValue>(result);  
}

ValuePtr abs(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber()){
        throw LispError("Cannot take the absolute value of a non-numeric value.");
    }
    double result = std::abs(static_cast<NumericValue*>(params[0].get())->asNumber());
    return std::make_shared<NumericValue>(result);  
}

ValuePtr odd(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber()){
        throw LispError("Cannot check if a non-numeric value is odd.");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(static_cast<int>(result) % 2 != 0);  
}

ValuePtr even(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber()){
        throw LispError("Cannot check if a non-numeric value is even.");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(static_cast<int>(result) % 2 == 0);  
}

ValuePtr zero(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber()){
        throw LispError("Cannot check if a non-numeric value is zero.");
    }
    double result = static_cast<NumericValue*>(params[0].get())->asNumber();
    return std::make_shared<BooleanValue>(result == 0);  
}

ValuePtr greater(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Cannot compare non-numeric values.");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 > result2);  
}

ValuePtr less(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw LispError("Wrong number of arguments.");
    }
    if(!params[0]->isNumber() || !params[1]->isNumber()){
        throw LispError("Cannot compare non-numeric values.");
    }
    double result1 = static_cast<NumericValue*>(params[0].get())->asNumber();
    double result2 = static_cast<NumericValue*>(params[1].get())->asNumber();
    return std::make_shared<BooleanValue>(result1 < result2);  
}

ValuePtr print(const std::vector<ValuePtr>& params){
    for(const auto& i: params){
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr isBool(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->isBoolean());
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

ValuePtr atom(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->isAtom());
}

ValuePtr isNull(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->isNil());
}   

ValuePtr isNumber(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->isNumber());
}

ValuePtr isPair(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::PAIR);
}

ValuePtr isProcedure(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::BUILTIN_PROC);
}

ValuePtr isSymbol(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->asSymbol().has_value());
}

ValuePtr isString(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->getType() == ValueType::STRING);
}

ValuePtr isInteger(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
    }
    return std::make_shared<BooleanValue>(params[0]->isInteger());
}

ValuePtr isList(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw LispError("Wrong number of arguments.");
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







