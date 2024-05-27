# 项目创新
## 实现文件模式：代码复用
为实现文件模式，实际上需要修改的只有传入代码的流对象类型以及解释器对于求值结果是否需要打印。为解决这一问题，我们新定义一个抽象了整个RE(P)L过程的函数**runInterpreter**。它接受一个字符串参数"FILE"或"REPL"，用于标识解释器当前处于何种模式，注意，这里我们不使用RTTI(运行时对象识别)，而采用较为简单直接的做法；接受一个std::istream流对象，它可以是标准输入流、文件流或字符串流，这是我们实现抽象和代码复用的关键；接受一个EvalEnv类指针，也即我们用来求值的环境。
```
void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env){
    while(true){
        if(mode == "REPL"){
            std::cout << ">>> ";
        }
        //Read
        std::string line;
        std::getline(input, line);
        ......
        //Evaluate
        ......
        if(mode == "REPL"){
            std::cout << result->toString() << std::endl;
        }
    }
}
```
另外，我们定义辅助函数**readFromFile**，它接受一个字符串参数"filename"，即文件路径，返回一个**std::istringstream**类型的对象，它将作为输入流对象被传入**runInterpreter**函数。这一步是为了处理文件中的空行等，使文件中的代码与命令行交互时我们输入的代码在形式上完全对齐。
最后，我们在**main**函数中进行总调度。根据用户在命令行输入的不同，传入不同参数调用**runInterpreter**函数，这样就实现了文件模式。
## 实现解释器换行功能
我们维护一个**int**类型的变量**openBrackets**，用于计数未匹配括号的个数，以此判断用户的输入是否结束。如果**openBrackets**的值大于0，那么说明还有未匹配的括号，代码应该继续读取下一行。如果**openBrackets**的值小于等于0，可以开始解析和执行这个代码块。注意，这里我们将右括号多余的情况留待解析时再抛出异常，无需在**runInterpreter**中做额外处理。
```
void runInterpreter(std::string mode, std::istream& input, std::shared_ptr<EvalEnv> env){
    std::string code;
    int openBrackets = 0;
    while(true){
        ......
        std::string line;
        std::getline(input, line);
        ......
        for(char& c : line){
            if(c == '(') openBrackets++;
            if(c == ')') openBrackets--;
        }
        code += line;
        ......
        if(openBrackets > 0){
            if(mode=="REPL") std::cout << "... ";
            continue;
        }
        else{
            //Start RE(P)
            ......
            //End RE(P)
            code = "";     
            openBrackets = 0; 
        }
        ......
    }
}
```
## 实现语法扩充
### **let\***
已有**letForm**函数的情况下，实现**let\***基本上是白盒复用，也即把大部分逻辑直接拷贝即可。唯一的修改是：在**letStarForm**函数里，我们创建了一个新的求值环境，它以传入**letStarForm**的环境为父环境，我们在进行变量绑定的for循环中迭代更新这个求值环境，每次都在前一个变量进行绑定的环境的子环境中进行新变量的绑定。
```
ValuePtr letStarForm(const std::vector<ValuePtr>& args, EvalEnv& e){
    ......
    auto child = e.shared_from_this();
    for (const auto& i: definitions){
        ......
        child = child->createChild({*def[0]->asSymbol()}, {vals.back()});
    }
    ......
}
```
### **delay**与**force**
为了实现惰性求值，我们需要一个新的类型的值：**promise**。为此，我们在**Value.h**和**Value.cpp**中补充了**PromiseValue**的声明和定义。一个**PromiseValue**类的对象持有两个核心成员变量，**bool**类型的**forced**和**ValuePtr**类型的**value**，前者用于标记该**promise**是否已被**force**过，后者存储着**promise**内部真正的值。同时，**PromiseValue**类定义了方法**force()**，它用于执行**promise**的求值，如果它尚未被求值过，那么进行求值并返回求值结果，同时将**value**更新为求值后的值；如果它已经被求值过，直接返回**value**。
然后，我们在**delayForm**函数里返回一个**PromiseValue**类对象，在**forceForm**函数里调用传入函数的**PromiseValue**类对象的**force()**方法。这就完成了**delay**和**force**的语法。
### 其他特殊形式
**case**与**do**特殊形式的实现虽然过程比较繁琐，但逻辑上十分简单，只需按照规约解析传入的**PairValue**类对象，把它拆解为特殊形式所需的部分，再依次对应逻辑即可。
在已有**define**特殊形式的前提下，**set\!**特殊形式的实现是平凡的。