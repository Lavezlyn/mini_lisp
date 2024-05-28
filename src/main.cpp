#include <iostream>
#include <fstream>
#include <sstream>
#include "value.h"
#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env);
std::istringstream readFromFile(const std::string& filename);

int main(int argc, char* argv[]) {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    if(argc == 1) runInterpreter("REPL", std::cin, env);
    else if(argc == 2){
        std::istringstream input = readFromFile(argv[1]);
        runInterpreter("FILE", input, env);
    } else {
        std::cerr << "Usage: " << argv[0] << " (optional)<filename>" << std::endl;
        std::exit(1);
    }
}

void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env){
    std::string code;
    int openBrackets = 0;
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
                if(code == "") continue;
                if(mode == "FILE" && code == "EndOfFile") std::exit(0);
                std::string codeCopy = code;
                code = "";
                auto tokens = Tokenizer::tokenize(codeCopy);
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
    content += "EndOfFile\n";
    return std::istringstream(content);
}