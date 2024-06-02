#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <ostream>

class EvalEnv;

enum class ValueType{
    BOOLEAN,
    NUMERIC,
    STRING,
    SYMBOL,
    NIL,
    PAIR,
    BUILTIN_PROC,
    LAMBDA,
    PROMISE
};

class Value;
using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class Value{
private:
    ValueType type;
protected:
    Value(ValueType type) : type{type} {}
public:
    virtual ~Value() = default;

    virtual bool isInteger() const = 0;
    virtual std::string toString() const = 0;
    ValueType getType() const {
        return type;
    }
    bool isBoolean() const {return type == ValueType::BOOLEAN;}
    bool isNil() const {return type == ValueType::NIL;}
    bool isNumber() const {return type == ValueType::NUMERIC;}
    bool isSelfEvaluating() const {
        return type == ValueType::BOOLEAN || type == ValueType::NUMERIC || type == ValueType::STRING || type == ValueType::BUILTIN_PROC || type == ValueType::LAMBDA || type == ValueType::PROMISE;
    }
    bool isAtom() const {
        return type == ValueType::BOOLEAN || type == ValueType::NUMERIC || type == ValueType::STRING || type == ValueType::SYMBOL || type == ValueType::NIL;
    }
    virtual std::optional<std::string> asSymbol() const {return std::nullopt;};
    virtual std::vector<ValuePtr> toVector() const {return {};};
};

class BooleanValue : public Value {
private:
    bool value;
public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {}

    bool getValue() const {return value;}
    bool isInteger() const override { return false; }
    std::string toString() const override;
};

class NumericValue : public Value {
private:
    double value;
public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}

    double asNumber() const {return value;}
    bool isInteger() const override {return value == static_cast<int>(value);}
    std::string toString() const override;
};

class StringValue : public Value {
private:
    std::string value;
public:
    StringValue(std::string value) : Value(ValueType::STRING), value{value} {}

    bool isInteger() const override { return false; }
    std::string getValue() const {return value;}
    std::string toString() const override;
};

class SymbolValue : public Value {
private:
    std::string value;
public:
    SymbolValue(std::string value) : Value(ValueType::SYMBOL), value{value} {}

    bool isInteger() const override { return false; }
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
    ValuePtr getCar() const {return car;}
    ValuePtr getCdr() const {return cdr;}
    void setCar(ValuePtr value) {car = value;}
    void setCdr(ValuePtr value) {cdr = value;}
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
    auto getFunc() const {return func;}
};
    
class LambdaValue : public Value {
private:
    std::vector<ValuePtr> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> parent;
public:
    LambdaValue(std::vector<ValuePtr> params, std::vector<ValuePtr> body, std::shared_ptr<EvalEnv> parent) : Value(ValueType::LAMBDA), params{params}, body{body}, parent{parent} {}
    bool isInteger() const override { return false; }
    std::string toString() const override;
    ValuePtr apply(const std::vector<ValuePtr>& args);
    std::vector<std::string> getParams() const;
};

class PromiseValue : public Value {
private:
    ValuePtr value;
    bool forced;
    std::shared_ptr<EvalEnv> env;
public:
    PromiseValue(ValuePtr value, std::shared_ptr<EvalEnv> env) : Value(ValueType::PROMISE), value{value}, forced{false}, env{env} {}

    bool isInteger() const override { return false; }
    std::string toString() const override;
    ValuePtr force();
};

#endif