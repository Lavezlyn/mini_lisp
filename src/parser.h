#ifndef PARSER_H
#define PARSER_H

#include <deque>
#include <memory>
#include <optional>
#include "./token.h"
#include "./value.h"

class Parser{
private:
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr>&& tks) : tokens{std::move(tks)} {}
    ValuePtr parse();
    ValuePtr parseTails();
};

#endif