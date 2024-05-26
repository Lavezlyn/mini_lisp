# Extra features
## 1 语法扩充
### (1) **let\***
#### 介绍
**let\*** 允许你创建一系列的局部绑定，**这些绑定可以依赖于前面的绑定**。这与 **let** 形式不同，**let** 形式的绑定是并行的，不能依赖于其他绑定。
#### 用法
```
>>> (let* ((x 2)
...        (y (* x 3)))  ; y 依赖于 x
...   (+ x y))  
8
```
### (2) **delay**与**force**
#### 介绍
**delay** 和 **force** 是用于实现惰性求值的两个特殊形式。delay 用于创建一个被延迟的表达式，也就是说，这个表达式在被创建时不会立即求值。**force** 用于求值一个被 **delay** 创建的被延迟的表达式。
#### 用法
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
用delay和force创建一个无限的自然数列表
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
### (3) **do**
#### 介绍
**do** 是一个用于进行循环的特殊形式。**do** 的语法如下：
```
(do ((variable initial next) ...)
    (test expression ...)
    command ...)
```
每个**do**特殊形式由三部分组成。第一部分是变量定义列表，变量的个数没有限制。对于每个变量定义，**variable** 是变量名，**initial**是变量的初始值，**next**是在每次循环开始时更新变量的方式（通常是一个表达式）。第二部分是退出循环的判定。**test**是一个表达式，当它的值为真时，循环结束；**expression**是在循环结束时执行的表达式，它的求值结果将作为**do**形式的返回值。第三部分是在每次循环时都要执行的循环体，即**command**，它的存在不是必要的，即允许**do**形式没有循环体。
#### 用法
```
>>> (define (sum-naturals n)
...   (do ((i 0 (+ i 1))
...        (sum 0 (+ sum i)))
...       ((>= i n) sum)))
()
>>> (sum-naturals 10)
55
```
### (4) **set!**
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
## 2 内置过程扩充
### (1) **promise?**
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
```
### (2) **set-car!**与**set-cdr!**
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
## 3 改善用户体验
现在，解释器支持换行。如果你尚未完成输入，按下**enter**键换行，你会看到解释器用于提示输入的标识>>>变成了...  。这意味着解释器正在等待你继续输入。同样的，文件模式中也支持换行，只是你不会看到提示符。
