---
title: ipc
date: 2019-07-10 14:09
categories: 
tags: 
---

## pipes
缺陷:
* 历史上,pipe是半双工的,虽然后来很多系统的管道都提供了全双工支持,但是我们没做调研的情况下,还是不建议默认作出这个假设
* 管道一般用于父子进程之间通行,使用过程:创建pipe，fork，然后父子通信

连接管道的文件描述符在fstat查看类型的时候,都变成了fifo类型
```
struct stat buf;
fstat(fd[0], &buf);
S_ISFIFO(buf.st_mode); //true
```
*如果连接管道的两端是默认STDOUT_FILENO STDIN_FILENO STDERR_FILENO ,那么管道创建后会被重新赋值,连接标准输入输出错误,所以他们永远都是字符设备S_ISCHR*
