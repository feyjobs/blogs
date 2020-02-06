---
title: C++
date: 2018-01-13 21:19
categories: 
- Code
- C
tags:
---

## 指针声明对象与直接对象声明的区别
废话不多说先上code
```C++
class A{
    public:
        A(int a){this->a = a};
    private:
        int a;
}

int main(){
    A a = A(1);
    A* b = new A(1);
}
```
a,b两种声明方式，一个有new,一个没有new,有什么区别呢。
答案: 直接用构造函数生成对象是在栈中分配了内存空间，而通过new关键词是在堆中分配了内存空间。
一个进程的栈的内存空间往往非常少，参见ulimit -s 查看系统的进程栈空间最大值，栈的分配与释放都是由操作系统来维护的，而堆的内存，也就是动态内存分配，是由程序员自己负责管理，申请与销毁的
栈是由机器系统提供的数据结构，计算机底层会提供对栈的支持，分配专门的寄存器存放栈的地址，压栈出栈都有专门的指令执行。而堆是由C/C++提供的，库函数根据特有算法进行动态申请与分配

## const
const本身描述一个事实,被这个修饰符修饰的对象不可以被修改,但是一加上指针和引用,逻辑就绕的没法玩了,梳理一下,梳理的目标主要围绕一下几点来
* 该const变量的声明要求
* 该const变量的赋值要求
*
### 最普通
```c++
const int a = 1;
```
* 必须初始化
* const修饰的变量不能修改

错误例子:
```c++
const int a;//没有初始化

const int b = 2;
b = 3;//错误,不允许修改
```

### const+ 引用
const+引用后,引用无法修改他所绑定的对象
```c++
int a;
const int &b = a; //声明一个常量引用 绑定一个非常量 表示不能通过该引用修改对象

const int a = 1;
const int &b = a;//声明一个常量引用 绑定一个常量 表示不能通过该引用修改对象
```
>对于引用而言,不允许引用的类型与对象类型不一致,但是常量引用是个例外,只要常量引用表达式的右边能转化成左边的类型就可以
```c++
int &a = 1;//非法

const int &a = 3.14;//合法 编译器会转化为下面代码
const int temp = 3.14;
const int &a =temp; //ri会绑定这个临时量

//为什么不是const就不行呢 如果像上面的一样
int &a = 3.14;
int temp = 3.14;
int &a = temp;//那a绑定的是临时变量,不是语法所以希望的3.14,  所以const引用允许这么声明是因为const引用不可能去修改对象的内容
```

错误例子:
```c++
const int a  = 1;
int &b = a; //不能用一个非常量引用绑定一个常量
```
**不能用非const引用绑定const变量**

### const+指针
#### 指向常量的指针
指向常量的指针
* 指针可以改
* 指针指向的内容不能该
```c++
const double pi = 3.14;
const double *ptr = &pi;

double c = 2333;
ptr = &c;//合法
```
怎么理解指向常量的指针呢,其实这个指针和普通的指针一点区别的没有,可以正常声明,可以正常获取,但是这个指针自己关闭了*ptr修改对象的能力,至于他指向的是不是一个常量,这玩意儿他并不在乎
这里必须说一下,以前对c/c++声明的理解有问题
原来我的理解
```c++
int *aa; //int是一种数据类型,*aa是一个变量主体
//正确的理解应该是一个变量的声明其实是 变量类型 + 声明符 + 变量名 这两种理解方式有什么区别
const double a;//错误a是一个常量,应该初始化
const double *a;//正确 a是一个普通的指针而已,这个指针不是常量,这个声明语句的主题应该是a,所以a不初始化一点关系没有,但是你如果用我原先的方式理解*,你就会纠结于
//*a为什么不用初始化
```

#### 常量指针
表示一个指针本身是个常量,他不能被修改
注意:引用本身不是个对象,所以我们说的常量引用指的是常量的引用,而不是说这个引用是个常量!
```c++
double pi = 3.14;
double *const ptr = &pi;//这个时候const修饰的已经是这个指针了,就必须要初始化了
```
### 顶层const
顶层:本身是个常量   顶层const在copy时不受影响,可以随便拷贝给别人
底层:指向的内容是个常量

