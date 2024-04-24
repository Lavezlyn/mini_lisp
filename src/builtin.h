#ifndef BUILTIN_H
#define BUILTIN_H

#include "./value.h"
#include <unordered_map>    

ValuePtr add(const std::vector<ValuePtr>& args);
ValuePtr sub(const std::vector<ValuePtr>& args);
ValuePtr mul(const std::vector<ValuePtr>& args);
ValuePtr divide(const std::vector<ValuePtr>& args);
ValuePtr print(const std::vector<ValuePtr>& args);
ValuePtr ABS(const std::vector<ValuePtr>& args);
ValuePtr expt(const std::vector<ValuePtr>& args); 
ValuePtr modulo(const std::vector<ValuePtr>& args);
ValuePtr quotient(const std::vector<ValuePtr>& args);
ValuePtr Remainder(const std::vector<ValuePtr>& args);
ValuePtr equal(const std::vector<ValuePtr>& args);
ValuePtr eq(const std::vector<ValuePtr>& args);
ValuePtr NOT(const std::vector<ValuePtr>& args);  
ValuePtr odd(const std::vector<ValuePtr>& args);
ValuePtr even(const std::vector<ValuePtr>& args);
ValuePtr zero(const std::vector<ValuePtr>& args);
ValuePtr numEq(const std::vector<ValuePtr>& args);
ValuePtr less(const std::vector<ValuePtr>& args);
ValuePtr greater(const std::vector<ValuePtr>& args);
ValuePtr lessEq(const std::vector<ValuePtr>& args);
ValuePtr greaterEq(const std::vector<ValuePtr>& args);
ValuePtr atom(const std::vector<ValuePtr>& args);
ValuePtr boolean(const std::vector<ValuePtr>& args);
ValuePtr number(const std::vector<ValuePtr>& args);
ValuePtr pair(const std::vector<ValuePtr>& args);
ValuePtr procedure(const std::vector<ValuePtr>& args);
ValuePtr symbol(const std::vector<ValuePtr>& args);
ValuePtr null(const std::vector<ValuePtr>& args);
ValuePtr string(const std::vector<ValuePtr>& args);
ValuePtr integer(const std::vector<ValuePtr>& args);
ValuePtr list(const std::vector<ValuePtr>& args);
ValuePtr append(const std::vector<ValuePtr>& args);
ValuePtr car(const std::vector<ValuePtr>& args);
ValuePtr cdr(const std::vector<ValuePtr>& args);
ValuePtr cons(const std::vector<ValuePtr>& args);
ValuePtr length(const std::vector<ValuePtr>& args);
ValuePtr makelist(const std::vector<ValuePtr>& args);
ValuePtr map(const std::vector<ValuePtr>& args);
ValuePtr filter(const std::vector<ValuePtr>& args);
ValuePtr reduce(const std::vector<ValuePtr>& args);

extern std::unordered_map<std::string, ValuePtr> BUILTIN;

#endif