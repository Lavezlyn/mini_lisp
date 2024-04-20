#ifndef BUILTIN_H
#define BUILTIN_H

#include "./value.h"

ValuePtr add(const std::vector<ValuePtr>& args);
ValuePtr sub(const std::vector<ValuePtr>& args);
ValuePtr mul(const std::vector<ValuePtr>& args);
ValuePtr divide(const std::vector<ValuePtr>& args);

#endif