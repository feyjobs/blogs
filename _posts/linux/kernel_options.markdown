---
title: kernel_options
date: 2021-04-20 13:58
categories: 
tags: 
---


## vm.swappiness
```
default: 60
recommend: 10
```
这个选项表示，系统内存使用到(100 - 60)%的时候就开始使用swap内存，swap内存也就是交换区内存, 用一部分磁盘来模拟内存, 理所当然得会变慢

## net.ipv4.tcp_rmem


