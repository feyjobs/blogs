---
title: Linux系统
date: 2017-09-12 11:28:10
categories: 
- Linux
tags:
---

## 文件系统
### 功能性文件
/proc/cpuinfo cpu相关信息的内容
### 文件权限
[filetype]rwxrwxrwx
ll之后的文件权限相关内容,
#### filetype
|类型|意义|
|:..:|:..:|
|-   |普通文件|
|d   |目录文件|
|l   |链接文件|
|b   |block 块设备文件,磁盘,光盘等,/dev下|
|c   |char 字符设备文件,一次传输一个字符的设备,键盘,字符终端等|
|p   |管道文件|
|s   |套接字文件|

#### rwx
每组rwx分别对应user:group:other的文件权限,

对于文件即**filetype=-**的文件
r权限为可读,w为可写权限,x为可执行权限(前提是文件是可执行的)

对于文件夹即***filetype=d*的文件夹
r权限为可以查看文件夹下文件/文件夹名,w为可以更改该文件夹下的文件,文件夹,x为可以进入改文件夹
