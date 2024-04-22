#include "./value.h"
#include <iomanip>
#include <string>
#include <sstream>
#include "./error.h"

std::string BooleanValue::toString() const {
    return value ? "#t" : "#f";
}

std::string NumericValue::toString() const {
    if(value == static_cast<int>(value))
        return std::to_string(static_cast<int>(value));
    else
        return std::to_string(value);
}

std::string StringValue::toString() const {
    std::ostringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::string SymbolValue::toString() const {
    return value;
}

std::string NilValue::toString() const {
    return "()";
}

std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}

std::string PairValue::toString() const {
    std::string result = "(" + car->toString();
    auto cdr = getCdr();
    while (cdr->getType() == ValueType::PAIR || cdr->getType() == ValueType::NIL){
        if (cdr->getType() == ValueType::NIL){
            result += ")";
            return result;
        }   
        auto pair = static_cast<PairValue*>(cdr.get());
        result += " " + pair->getCar()->toString();
        cdr = pair->getCdr();
    }
    result += " . " + cdr->toString() + ")";
    return result;
}

std::vector<ValuePtr> PairValue:: toVector() const {
    try{
    std::vector<ValuePtr> result;
    result.push_back(this->car);
    auto cdr = getCdr();
    while (cdr->getType() == ValueType::PAIR || cdr->getType() == ValueType::NIL){
        if (cdr->getType() == ValueType::NIL){
            return result;
        }
        auto pair = static_cast<PairValue*>(cdr.get());
        result.push_back(pair->getCar());
        cdr = pair->getCdr();
    }
    result.push_back(cdr);
    return result;
    }catch(...){
        throw RuntimeError("Error in toVector");
    }
}


