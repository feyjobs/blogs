---
title: apue-proc
date: 2018-09-03 11:39
categories: 
tags: 
---

## 程序启动流程
编译器=>链接器=>启动例程(获取命令行参数,环境变量)=>启动main函数=>启动历程调用exit(获取main函数返回值)

## 进程退出方式
## 退出函数
```c
//ISO C函数
# include <stdlib.h>
void exit(int  status);  //自动调用标准io库的清理关闭操作 fclose
void _Exit(int status);

//POSIX.1函数
#include <unistd.h>
void _exit(int status);
```

# 进程控制
* fork之后写时复制 将愿text区 数据区 堆栈区置为只读,每个进程的区域有所变化，单独申请一个备份页进行替换
* fork之后 父子进程共享打开的文件表,所以文件偏移量也会共享
*
