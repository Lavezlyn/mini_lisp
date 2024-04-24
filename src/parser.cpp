#include "./parser.h"
#include "./error.h"
#include "./token.h"    
#include "./value.h"    
#include <stdexcept>
#include <memory>
#include <deque>
#include <iostream>

ValuePtr Parser::parse(){
    if(tokens.empty()){
        throw SyntaxError("Unexpected end of file");
    }
    auto token = tokens.front().get();
    if(token->getType() == TokenType::NUMERIC_LITERAL){
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<NumericValue>(value);
    }
    if(token->getType() == TokenType::BOOLEAN_LITERAL){
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<BooleanValue>(value);
    }
    if(token->getType() == TokenType::STRING_LITERAL){
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<StringValue>(value);
    }
    if(token->getType() == TokenType::IDENTIFIER){
        auto value = static_cast<IdentifierToken&>(*token).getName();
        tokens.pop_front();
        return std::make_shared<SymbolValue>(value);
    }
    if (token->getType() == TokenType::QUOTE) {
    tokens.pop_front();
    return std::make_shared<PairValue>(
      std::make_shared<SymbolValue>("quote"),
      std::make_shared<PairValue>(
          this->parse(),
          std::make_shared<NilValue>()
      )
    );
    }
    if (token->getType() == TokenType::QUASIQUOTE) {
    tokens.pop_front();
    return std::make_shared<PairValue>(
      std::make_shared<SymbolValue>("quasiquote"),
      std::make_shared<PairValue>(
          this->parse(),
          std::make_shared<NilValue>()
      )
    );
    }
    if (token->getType() == TokenType::UNQUOTE) {
    tokens.pop_front();
    return std::make_shared<PairValue>(
      std::make_shared<SymbolValue>("unquote"),
      std::make_shared<PairValue>(
          this->parse(),
          std::make_shared<NilValue>()
      )
    );
    }
    if (token->getType() == TokenType::LEFT_PAREN){
        tokens.pop_front();
        return this->parseTails();
    }
    else{
        throw SyntaxError("Invalid token");
    }
}

ValuePtr Parser::parseTails(){
    if(tokens.empty()){
        throw SyntaxError("Unexpected end of file");
    }
    if(tokens.front()->getType() == TokenType::RIGHT_PAREN){
        tokens.pop_front();
        return std::make_shared<NilValue>();
    } 
    auto car = this->parse();
    if(tokens.empty()){
        throw SyntaxError("Unexpected end of file");
    }
    if(tokens.front()->getType() == TokenType::DOT){
        tokens.pop_front();
        auto cdr = this->parse();

        if(tokens.front()->getType() != TokenType::RIGHT_PAREN) {
        throw SyntaxError("Invalid token after dot. Expect a right parenthesis");
       }

        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    }
    else{
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
   }
}