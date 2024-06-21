#include <iostream>
#include <fstream>
#include <sstream>
#include "value.h"
#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"
#include "./error.h"

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env);
std::istringstream readFromFile(const std::string& filename);

int main(int argc, char* argv[]){
    auto env = EvalEnv::createGlobal();
    if (argc == 2){
        std::istringstream file = readFromFile(argv[1]);
        runInterpreter("FILE", file, env);
    }
    else if (argc == 1)
        runInterpreter("REPL", std::cin, env);
    else
        std::cout << "Usage: ./mini_lisp [filename]" << std::endl;
    return 0;
}

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env){
    std::string inputBuffer;
    int openBrackets = 0;   
    bool isFirstLine = true;

    while (true){
        try{
            if (mode == "REPL"){
                if (isFirstLine){
                    std::cout << ">>> ";
                    isFirstLine = false;
                }
                else std::cout << "... ";
            }

            std::string line;
            if (!std::getline(input, line)){
                if(input.eof() && openBrackets == 0)
                    break;
                else
                    throw SyntaxError("Unbalanced parentheses");
            }

            inputBuffer += line + "\n";
            
            auto tokens = Tokenizer::tokenize(line);
            for (const auto& token : tokens){
                if (token->getType() == TokenType::LEFT_PAREN)
                    openBrackets++;
                else if (token->getType() == TokenType::RIGHT_PAREN)
                    openBrackets--;
            }

            if (openBrackets == 0){
                auto tokens = Tokenizer::tokenize(inputBuffer);
                Parser parser(std::move(tokens));
                auto value = parser.parse();
                auto result = env->eval(std::move(value));  
                if (mode == "REPL")
                    std::cout << result->toString() << std::endl;
                inputBuffer = "";
                isFirstLine = true;
            }
            else if (openBrackets < 0){
                throw SyntaxError("Unbalanced parentheses");
            }
        }catch(std::runtime_error& e){
            inputBuffer = "";
            openBrackets = 0;
            isFirstLine = true;
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

std::istringstream readFromFile(const std::string& filename){
    std::ifstream file(filename, std::ios::in);
    std::string content;
    if(!file.is_open()){
        std::cerr << "Error: Could not open file " << filename << std::endl;
        std::exit(1);
    }
    std::string line;
    while(std::getline(file, line)){
        if(line == "") continue;
        content += line + "\n";
    }
    return std::istringstream(content);
}