#include <iostream>
#include <string>
#include <fstream>
#include "value.h"
#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env);

int main(int argc, char* argv[]) {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    if(argc == 1) runInterpreter("REPL", std::cin, env);
    else if(argc == 2){
        std::ifstream file(argv[1]);
        if(file.is_open())
            runInterpreter("FILE", file, env);
        else {
            std::cerr << "Error: Could not open file." << std::endl;
            std::exit(1);
        }
    } else {
        std::cerr << "Usage: " << argv[0] << " (optional)<filename>" << std::endl;
        std::exit(1);
    }
}

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env){
    while(true){
        try{
            if(mode == "REPL")
                std::cout << ">>> ";
            std::string line;
            std::getline(input, line);
            if(input.eof()){
                if(mode == "REPL") std::exit(0);
                else {mode = "FILEend";}
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            if(mode == "REPL")
                std::cout << result->toString() << std::endl;
            if(mode == "FILEend") std::exit(0);
        } catch (std::runtime_error& e){
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}