#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include "./value.h"
#include <unordered_map>

using ValuePtr = std::shared_ptr<Value>;
class EvalEnv : public std::enable_shared_from_this<EvalEnv>{
private:
    std::unordered_map<std::string, ValuePtr> env;
    std::shared_ptr<EvalEnv> parent {nullptr};    
    EvalEnv(std::shared_ptr<EvalEnv> parent);
public:
    static std::shared_ptr<EvalEnv> createGlobal();
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
    ValuePtr lookupBinding(const std::string& name);
    void defineBinding(const std::string& name, ValuePtr value);   
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
};

#endif