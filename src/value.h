#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <ostream>

enum class ValueType{
    BOOLEAN,
    NUMERIC,
    STRING,
    SYMBOL,
    NIL,
    PAIR,
    BUILTIN_PROC,
    LAMBDA
};

class Value;
using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);

class Value{
private:
    ValueType type;
protected:
    Value(ValueType type) : type{type} {}
public:
    virtual ~Value() = default;

    ValueType getType() const {
        return type;
    }

    bool isBoolean() const {
        return type == ValueType::BOOLEAN;
    }
    virtual bool isInteger() const = 0;
    bool isNil() const {
        return type == ValueType::NIL;
    }
    bool isNumber() const {
        return type == ValueType::NUMERIC;
    }
    bool isSelfEvaluating() const {
        return type == ValueType::BOOLEAN || type == ValueType::NUMERIC || type == ValueType::STRING || type == ValueType::BUILTIN_PROC;
    }
    bool isAtom() const {
        return type == ValueType::BOOLEAN || type == ValueType::NUMERIC || type == ValueType::STRING || type == ValueType::SYMBOL || type == ValueType::NIL;
    }

    virtual std::optional<std::string> asSymbol() const {
        return std::nullopt;
    };

    virtual std::vector<ValuePtr> toVector() const {return {};};
    virtual std::string toString() const = 0;
};

class BooleanValue : public Value {
private:
    bool value;
public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {}

    bool getValue() const {
        return value;
    }
    bool isInteger() const override { return false; }
    std::string toString() const override;
};

class NumericValue : public Value {
private:
    double value;
public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}

    double asNumber() const {
        return value;
    }
    bool isInteger() const override { 
        return value == static_cast<int>(value);
     }
    std::string toString() const override;
};

class StringValue : public Value {
private:
    std::string value;
public:
    StringValue(std::string value) : Value(ValueType::STRING), value{value} {}

    bool isInteger() const override { return false; }
    std::string getValue() const {
        return value;
    }
    std::string toString() const override;
};

class SymbolValue : public Value {
private:
    std::string value;
public:
    SymbolValue(std::string value) : Value(ValueType::SYMBOL), value{value} {}

    bool isInteger() const override { return false; }
    std::string getValue() const {
        return value;
    }
    std::string toString() const override;
    std::optional<std::string> asSymbol() const override { return value; }
};

class NilValue : public Value {
public:
    NilValue() : Value(ValueType::NIL) {}

    bool isInteger() const override { return false; }
    std::string toString() const override;
};

class PairValue : public Value {
private:
    ValuePtr car;
    ValuePtr cdr;
public:
    PairValue(ValuePtr car, ValuePtr cdr) : Value(ValueType::PAIR), car{car}, cdr{cdr} {}

    bool isInteger() const override { return false; }
    ValuePtr getCar() const {
        return car;
    }
    ValuePtr getCdr() const {
        return cdr;
    }
    std::string toString() const override;
    std::vector<ValuePtr> toVector() const override;
};

class BuiltinProcValue : public Value {
private:
    BuiltinFuncType* func;
public:
    BuiltinProcValue(BuiltinFuncType* f) : Value(ValueType::BUILTIN_PROC), func{f} {}
    
    bool isInteger() const override { return false; }
    std::string toString() const override;
    auto getFunc() const {
        return func;
    }
};
    
class LambdaValue : public Value {
private:
    std::vector<ValuePtr> params;
    std::vector<ValuePtr> body;
public:
    LambdaValue(std::vector<ValuePtr> params, std::vector<ValuePtr> body) : Value(ValueType::LAMBDA), params{params}, body{body} {}
    bool isInteger() const override { return false; }
    std::string toString() const override;
};

#endif