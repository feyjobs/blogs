---
title: lua
date: 2020-02-11 00:42
categories: 
tags: 
---

# c api
## userdata
* ~~lua程序设计 userdata部分4/5(轻量级用户数据没看)~~
* __gc
* upvalue





# 注意点
* 写lua测试程序的时候, lua测试程序命名array.lua, 动态库测试程序array.so 在array.lua中require "array"的时候, 先找到了array.lua然后造成loop引用
* lua程序调用c库时, 参数的顺序值得注意a(a, b, c)，在lua栈上第一个位置时a, 第二个位置是b，一次类推, 按照压栈的顺序, 顺便提一下lua栈的顺序, 自底向上依次1-n, 自顶向下-1--n
