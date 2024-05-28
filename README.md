# Extra features
## 语法扩充
### **let\***
**let\*** 允许你创建一系列的局部绑定，**这些绑定可以依赖于前面的绑定**。这与 **let** 形式不同，**let** 形式的绑定是并行的，不能依赖于其他绑定。
```
>>> (let* ((x 2)
...        (y (* x 3)))  ; y 依赖于 x
...   (+ x y))  
8
```
也可以用let*实现快速排序。在这里，less和greater的定义都用到了先前定义的pivot和rest。
```
>>> (define (quicksort lst)
... (if (null? lst)
...     '()
...     (let* ((pivot (car lst))
...            (rest (cdr lst))
...            (less (filter (lambda (x) (< x pivot)) rest))
...            (greater (filter (lambda (x) (>= x pivot)) rest)))
...       (append (quicksort less) (list pivot) (quicksort greater)))))
()
>>> (quicksort '(12 71 2 15 29 82 87 8 18 66 81 25 63 97 40 3 93 58 53 31 47))
(2 3 8 12 15 18 25 29 31 40 47 53 58 63 66 71 81 82 87 93 97)
```
### **delay**与**force**
**delay** 和 **force** 是用于实现惰性求值的两个特殊形式。delay 用于创建一个被延迟的表达式，也就是说，这个表达式在被创建时不会立即求值。**force** 用于求值一个被 **delay** 创建的被延迟的表达式。
```
>>> (define delayed-sum (delay (+ 1 2)))  ; 创建一个被延迟的表达式
()
>>> delayed-sum
#<Promise (not forced)>
>>> (force delayed-sum)  ; 求值被延迟的表达式
3
>>> delayed-sum
#<Promise (forced)>
```
用**delay**和**force**创建一个无限的自然数列表
```
>>> (define (naturals n)
...   (cons n (delay (naturals (+ n 1)))))
()
>>> (define (force-nth stream n)
...   (if (= n 0)
...       (car stream)
...       (force-nth (force (cdr stream)) (- n 1))))
()
>>> (define nums (naturals 0))
()
>>> (force-nth nums 5)  
5
>>> (force-nth nums 100)  
100
```
### **do**
**do** 是一个用于执行循环的特殊形式。**do** 的语法如下：
```
(do ((variable initial next) ...)
    (test expression ...)
    command ...)
```
每个**do**特殊形式由三部分组成。第一部分是变量定义列表，变量的个数没有限制。对于每个变量定义，**variable** 是变量名，**initial**是变量的初始值，**next**是在每次循环开始时更新变量的方式（通常是一个表达式）。第二部分是退出循环的判定。**test**是一个表达式，当它的值为真时，循环结束；**expression**是在循环结束时执行的表达式，它的求值结果将作为**do**形式的返回值。第三部分是在每次循环时都要执行的循环体，即**command**，它的存在不是必要的，即允许**do**形式没有循环体。
```
>>> (define (sum-naturals n)
...   (do ((i 0 (+ i 1))
...        (sum 0 (+ sum i)))
...       ((>= i n) sum)))
()
>>> (sum-naturals 10)
55
```
### **case**
**case**特殊形式是一个条件结构，根据一个表达式的值来选择执行的分支，**case**的语法如下：
```
(case key
  ((datum1 ...) expression1 ...)
  ((datum2 ...) expression2 ...)
  ...
  (else default-expression ...))
```
**key**是一个表达式，它的值将被用来选择要执行的分支。每个**datum**是一个字面量（也即**atom?**判定为真的值），它们在每个分支的开头被列出。如果**key**的值等于某个**datum**，那么对应的**expression**就会被执行，它求值的结果将作为**case**形式的返回值。默认分支用**else**关键字声明，如果**key**的值没有匹配任何**datum**，那么**default-expression**就会被执行。注意，默认分支的存在不是必须的，如果没有一个分支被匹配，**case**形式什么也不做，返回空表。
```
>>> (define (describe x)
...   (case x
...     ((1) "one")
...     ((2) "two")
...     (else "other")))
()
>>> (describe 1)  
"one"
>>> (describe 2)
"two"
>>> (describe 10) 
"other"
```
### **set!**
**set!** 是一个用于重设变量的值的特殊形式。**set!** 的语法如下：
```
>>> (define x 2)
()
>>> x
2
>>> (set! x 3)
>>> x
3
```
## 内置过程扩充
### **promise?**
用于检验表达式的值是否是一个promise。
```
>>> (define x (delay (+ 1 2)))
()
>>> (promise? x)
#t
>>> (force x)
3
>>> (promise? x)
#t
>>> (promise? 2)
#f
```
### **set-car!**与**set-cdr!**
用于更改Pair的car和cdr，返回值默认为空表，如果操作的对象不是对子，解释器会抛出一个错误。
```
>>> (define pair (cons 1 2))
()
>>> (car pair)
1
>>> (cdr pair)
2
>>> (set-car! pair 3)
()
>>> (set-cdr! pair 4)
()
>>> (car pair)
3
>>> (cdr pair)
4
```
## 改善用户体验
现在，解释器支持换行。如果你尚未完成输入，按下**enter**键换行，你会看到解释器用于提示输入的标识>>>变成了... 

这意味着解释器正在等待你继续输入。同样的，文件模式中也支持换行，只是你不会看到提示符。