---
title: 套接字编程
date: 2019-1-17 11:44:39
categories: 
- apue
tags: 
---

## 套接字描述符
类似文件描述符,类似于套接字id
```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```