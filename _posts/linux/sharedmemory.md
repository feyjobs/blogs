---
title: 共享内存
date: 2019-07-17 14:02
categories: 
tags: 
- linux
---

总结几个共享内存的使用方式

## mmap

### /dev/zero方式
```c
 fd = open("/dev/zero", O_RDWR);
 
 if (fd == -1) {
     ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                   "open(\"/dev/zero\") failed");
     return NGX_ERROR;
 }
 
 shm->addr = (u_char *) mmap(NULL, shm->size, PROT_READ|PROT_WRITE,
                             MAP_SHARED, fd, 0);
```
* 需要设置MAP_SHARED选项,申请大小为size向上取页大小的最小倍数
* 内存区块初始化为0
* 父子进程之间可以共享

### 匿名方式
```c
shm->addr = (u_char *) mmap(NULL, shm->size,
                             PROT_READ|PROT_WRITE,
                             MAP_ANON|MAP_SHARED, -1, 0);
```
和上面类似,把fd设置成-1就可以
*上面两种方式都只能进行父子进程之间进行交互*

### 普通文件映射
```
void *ptr = mmap(NULL,  20, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
```
通过映射实际存在的同一个文件,毫不相关的进程之间也可以通过这种方式共享一块内存

## xsi共享内存
这个是xsi提供的共享内存方式
用起来比较麻烦而且沙雕
```c
key_t key = ftok("path", 1);
int shmId = shmget(key, 10, 0644|IPC_CREAT);
void *ptr = shmat(shmId, 0, 0);
```
*ftok*函数用来根据输入路径创建key,然后shmget接受key,申请内存大小,以及flag创建共享内存id
key的意义在于方便不同的进程查找获取某一个共享内存
flag主要包括了权限系统(与文件系统类似,用户读写,组读写,其他读写当前共享内存的能力),IPC_CREAT表示如果不存在该共享内存,则创建,IPC_EXCL表示如果当前共享内存已经存在,则报错

### IPC_PRIVATE
```c
int shmId = shmget(IPC_PRIVATE, 10, 0644|IPC_CREAT);
```
使用*IPC_PRIVATE*方式调用shmget每次必然创建一个新的共享内存,可以通过fork子进程依然感知到,但是不方便传输给其他无关进程,一般用于父子进程传输



