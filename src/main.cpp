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
    int openBrackets = 0;
    std::string code;
    while(true){
        try{
            if(mode == "REPL" && openBrackets == 0)
                std::cout << ">>> ";
            std::string line;
            std::getline(input, line);
            if(input.eof()){
                if(mode == "REPL") std::exit(0);
                else {mode = "FILEend";}
            }

            for(char& c : line){
                if(c == '(') openBrackets++;
                if(c == ')') openBrackets--;
            }
            code += line;

            if(openBrackets > 0){
                if(mode=="REPL") std::cout << "... ";
                else if(mode=="FILEend"){
                    std::cerr << "Error: Unexpected EOF" << std::endl;
                    std::exit(1);
                }
                continue;
            }
            else{
                auto tokens = Tokenizer::tokenize(code);
                code = "";
                Parser parser(std::move(tokens));
                auto value = parser.parse();
                auto result = env->eval(std::move(value));
                if(mode == "REPL")
                    std::cout << result->toString() << std::endl;
            }

            if(mode == "FILEend") std::exit(0);
        } catch (std::runtime_error& e){
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}