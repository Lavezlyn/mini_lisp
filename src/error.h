#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

class SyntaxError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class LispError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class RuntimeError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class ArgumentError : public LispError {
public:
    using LispError::LispError;
    ArgumentError() : LispError("Incorrect number of arguments") {}
};

#endif