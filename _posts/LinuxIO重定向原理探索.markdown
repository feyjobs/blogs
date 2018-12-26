---
title: LinuxIO重定向原理探索
date: 2018-08-30 13:57
categories: 
tags: 
---

在看apue的文件io部分,讲到lseek函数的时候,测试一段代码
```c
#include "apue.h"

int 
main(void){
    if(lseek(STDIN_FILENO, 0, SEEK_CUR) == -1){
        printf("cannot seek\n");
    }else{
        printf("seek ok\n");
    }
}
```
在测试这段代码时,作者使用了以下命令
```shell
./a.out < /etc/passwd
cat < /etc/passwd|./a.out
./a.out < /var/spool/cron.FIFO
```
一开始我就很不理解,代码中不是写死了从STDIN_FILENO读取吗,为什么还是会有不同的结果,

根本原因在于,把文件描述符和文件混淆在了一起,我们看一下linux文件描述符真正生效的原理
![-name](/uploads/apue/FDT.png)
进程描述表包含了该进程的文件描述表,里面包含了各个文件描述符,对应的也就是一串数字(文件描述符),我们的STDIN_FILENO,STDOUT_FILENO等等都只是对应的宏,这个只是在
语言级别的,到了系统根本就不管标准输入,标准输出,就是012,只是在初始化的时候就设置成了对应,标准输入输出,在重定向过程中,系统将该012等文件描述符对应为其他文件,于是就有了书上上面的操作。
