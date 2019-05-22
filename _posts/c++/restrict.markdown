---
title: restrict
date: 2019-1-28 17:12:25 
categories: 
tags: 
---

# restrict
一个c语言关键词,第一次看到是在apue上一直有这个关键词
```c
const char *inet_pton(int domain, const char* restrict str, void* restrict addr);
const char *inet_ntop(int domain, const void* restrict addr, char* restrict str, socklen_t size);
```
restrict关键词的意义是
> 告诉编译器,这个指针是这个对象的唯一指针,放心大胆开优化,但是傻逼开发要是不遵循这个约定,会干出啥事 不清楚
