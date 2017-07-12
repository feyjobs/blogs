---
title: PHP小case
date: 2017-02-03 17:43:10
categories: 
- Code
- PHP
tags:
---
#### 闭包
PHP闭包函数如果要使用外部变量主要要加上 use关键词/(ㄒoㄒ)/~~,又特么浪费时间
```
      int $sky = 1;
    array_filter($fairys,function love($fairy) use($sky) {
        echo $sky;
    });
```
不加use在闭包函数内,这个$sky就是个NULL.

#### tpl
一个上古老古董,百度居然还在用,我老天。
介绍下,首先这是个前后端分离用的,后端通过tpl的形式提供数据,然后通过模板整合页面与数据,然后一起生成一个页面