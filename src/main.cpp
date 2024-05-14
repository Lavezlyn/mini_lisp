#include <iostream>
#include <string>
#include "value.h"
#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"
#include "rjsj_test.hpp"
struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};

int main() {
    RJSJ_TEST(TestCtx, Lv7Lib);
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    while (true) {
        try {
            std::cout << ">>> " ;
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens)); 
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

//FixMe:(define (cube x) (* x x x)) => () ok
//      (define (sum term a next b) (if (> a b) 0 (+ (term a) (sum term (next a) next b)))) => () ok        
//      (define (inc n) (+ n 1)) => () ok
//      (define (sum-cubes a b) (sum cube a inc b)) => () ok
//      (sum-cubes 1 10)
//Program crashed