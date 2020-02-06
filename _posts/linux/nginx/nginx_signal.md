---
title: nginx signal
date: 2019-05-16 15:24
categories: 
tags: 
- nginx
- apue
- signal
---

# 总起
nginx提供了很多一些信号功能,以提供用户在进程运行时对当前进程进行操作,主要包含一下命令
**master**
* QUIT
* TERM
* WINCH
* HUP
* USR1
* USR2

## 信号
    信号提供了一种软终端机制,很多优秀的软件都提供了信号处理方法.信号提供了一种异步事件的处理方法
,无论是正在执行的程序需要杀死还是管道连接程序的下游需要提前推出
    在unix早期就像用户提供了信号机制,但是其并不可靠,例如Version 7.可能丢失信号,进入临界区代码时无法
关闭所选的信号。终于在BSD4.3版本和SRV3版本修正了这些问题,提供了稳定信号,遗憾的是他们二者并不兼容,
在Posix.1标准中可靠信号标准,这也是接下来所要描述的内容。
    在下文中,我们将要描述集中常用信号.然后我们会审视以往信号的实现问题,从而加深对信号的理解,

