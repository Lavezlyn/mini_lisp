#include "./builtin.h"
#include "./error.h"

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