---
title: Shell
date: 2017-08-01 19:00:00
categories: 
- Code
tags:
---

## 变量
### 位置参数

|变量|意义|
| :-------- | --------:|
|$0|当前脚本文件名|
|$n|传递给脚本或者函数的参数|
|$#|传递给脚本或者函数的参数个数|
|$?|命令的退出状态0表示没有错误,非零表示有错误|
|$@|传递到脚本的所有参数|
|$$|当前脚本的进程号|
|$0|当前脚本文件名|

### 本地变量
变量赋值

```shell
variable=value
echo ${variable=value}
```
> **shell语句中等号两边绝对不能加空格！！！** 
> ${variable=value}只能用在语句内部 单独成行报错

变量替换

```shell
$variable
${variable}
```
>推荐使用第二种方法,因为**如果变量名后面要跟如下内容：非小写字符串（包括大小字符串）、数字或下划线，那么变量名就一定要用大括号包起来**

### 引用
引用的作用:引用时**屏蔽特殊字符的特殊意义**，而将其解释为字面意义。也可以将引用理解为屏蔽。
|引用|意义|
| :-------- | --------:|
|""|引用除美元符号（$）、反引号（`）和反斜线（\）之外的所有字符|
|''|引用所有字符|
|``|解析为系统命令|
|\\|屏蔽下一个字符的特殊意义|

## 函数
定义方式:

```shell
function 函数名() {
    语句
    [return]
}
```
通过位置变量传递参数$n作用域仅仅为**函数内部**

```shell
#!/bin/bash
function show() {
    echo "hello , you are calling the function  $1"
}
echo "first time call the function"
show first
echo "second time call the function"
show second
```

函数的调用一定要在声明之后

## 判断
if语句基本格式
```shell
if [ command ];then
	command
elif [ command ];then
	command
fi
```
>注意 **command两边都是空格**,最后一定要加上fi,mmp应该是[]两边都必须是空格  烦死了 

### 文件/文件夹判断

|命令|意义|
| :-------- | --------:|
|-b|file存在,且是块特殊文件则为真|
|-c|file存在,且是字特殊文件则为真|
|-d|dir存在,且是一个目录则为真|
|-e|文件/文件夹存在则为真|
|-f|文件存在则为真|
|-f|文件存在则为真|

## 循环
### while
#### 计数器形式
```shell
#!/bin/shell

sum=0

i=1

while(( i <= 100))
do
	let "sum+=i"
	let "i += 2"
done 
echo sum=$sum
```

### for

### until


## ps
