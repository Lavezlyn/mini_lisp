#ifndef BUILTIN_H
#define BUILTIN_H

#include "./eval_env.h"
#include "./error.h"

ValuePtr add(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr sub(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr mul(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr divide(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr printer(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr ABS(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr expt(const std::vector<ValuePtr>& args, EvalEnv& env); 
ValuePtr modulo(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr quotient(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr Remainder(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr equal(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr eq(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr NOT(const std::vector<ValuePtr>& args, EvalEnv& env);  
ValuePtr odd(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr even(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr zero(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr numEq(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr less(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr greater(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr lessEq(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr greaterEq(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr atom(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr boolean(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr number(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr pair(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr procedure(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr symbol(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr null(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr string(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr integer(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr list(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr append(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr car(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr cdr(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr cons(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr length(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr makelist(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr map(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr filter(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr reduce(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr setCar(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr setCdr(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr promise(const std::vector<ValuePtr>& args, EvalEnv& env);

extern std::unordered_map<std::string, ValuePtr> BUILTIN;

#endif