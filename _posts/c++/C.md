---
title: C
date: 2017-12-16 11:28:10
categories: 
- Code
- C
tags:
---

# 常用宏
## offsetof
```
#define offsetof(type,member)((size_t)&((type*)0)->member)
```
解析:
0
(type*)0    强行将0作为type类型的指针
((type*)0)->member    获取member对象
&((type*)0)->member         获取membe对象地址
(size_t)&((type*)0)->member     将地址转化为size_t
## 文件I/O
### 文件
#### 文本视图和二进制视图
>通常对于文本文件使用文本视图,对二进制文件使用二进制视图,但是对于同一个文件,其存储的内容是一致的,关键在于怎么解析看待这个文件,一个文本文件也可以解析为二进制文件

![各个文件视图](/uploads/C/文本视图与二进制视图.png)

> 在文本视图中,程序所看到的内容和文件的内容会有所不同

解释: C语言会对当前操作系统环境对文本信息做不同的解析,如写入一个换行符都写入了\n,如果操作系统是windows则是写入\n\r,linux则是\r

#### I/O级别
* 低级别I/O,由操作系统提供的I/O服务
* 高级别I/O，由ANSI C提供的C语言标准I/O服务

优劣势：
* 标准I/O提供多个专用函数
* 标准I/O自带缓冲区

#### 标准文件
* 标准输入 stdin      读取默认的输入设备(p.s.键盘)
* 标准输出 stdout     写入默认的标准输出(p.s.显示屏)
* 标准错误 stderr     写入默认的标准输出(p.s.显示屏)

#### 函数众
说道这里,有点无聊,来个程序先嗨一发,
```C
#include <stdio.h>

#include <stdlib.h>

int main(int argc, char* argv[]) {
    int ch;
    FILE* fp;
    long count;

    if(argc != 2) {
        printf("intput the file u want to open!");
        exit(0);
    }
    if((fp = fopen(argv[1],"r")) == NULL){
        printf("open file %s failed!", argv[1]);
        exit(0);
    }

    while((ch = getc(fp)) != EOF) {
        putc(ch, stdout);
        count++;
    }
    fclose(fp);
    printf("File %s has %d characters",argv[1], count);
    exit(0);
}
```
这个程序是干什么的,
* 从命令行接收参数,参数为要打开的文件
* 打开相应的文件,将文件内容再输出到标准输出里,并统计文件内的字符数
运行结果:
./count count.c
![各个文件视图](/uploads/C/count_run.png)
##### fopen
一本正经的函数声明:
```C
FILE * fopen(const char * path, const char * mode);
```
###### 传参说明
path:很easy，文件路径,默认在当前执行文件路径查找
| mode      |     意义|
| :-------- | --------:|
| r|   可读|
| w|   可写,可创建|
| a|   可追加,可创建|
| r+|   可读写|
| w+|   可读写,若问件存在,则初始化为0|
| a+|   可读追加写,可以新建|
| rb,wb,ab,ab+,a+b,wb+,w+b,ab+,a+b|   类似于上面,二进制模式|
额 好多,剩下的自己查
>p.s. 注意带有<font color=red>w</font>的都会把原先的内容清空掉,谨慎,谨慎
###### 返回参数说明
FILE类型
* 文件指针
* 不指向实际文件,而是指向一个文件的数据包(包含文件的各种信息,包括缓冲区,)
以上为C primer pluse的解释 那么FILE到底是如何定义的呢！
找到系统/usr/include/stdio.h下
```C
typedef struct _IO_FILE FILE;
```
将struct _IO_FILE 解析成了FILE再追,查到_IO_FILE在libio.h里
```C
struct _IO_FILE {
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags

  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  /* The following fields are used to support backing up and undo. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
#if 0
  int _blksize;
#else
  int _flags2;
#endif
  _IO_off_t _old_offset; /* This used to be _offset but it's too small.  */

#define __HAVE_COLUMN /* temporary */
  /* 1+column number of pbase(); 0 is unknown. */
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

  /*  char* _save_gptr;  char* _save_egptr; */

  _IO_lock_t *_lock;
#ifdef _IO_USE_OLD_IO_FILE
};
```
好吧，有点小复杂,不深究了。

##### getc putc
```C
int getc(FILE *stream)
int fputc(int ch,FILE*fp)
```
与getchar() putchar()类似 就是把输入输出可以重定位了,不只是标准输入输出

>文件末尾的问题:getc()在读取一个文件末尾的时候,将会返回特殊值EOF,所以C语言只有在读到超过文件末尾的时候才能判断文件的末尾,所以为了避免读到空文件,对文件末尾的判断要加在循环开始的地方

##### puts
```C
int puts(const char *string);
```
将字符串输出到标准输出上
##### fclose()
```C
int fclose( FILE *fp );
```
返回值 0关闭成功 其余关闭失败
关闭一个文件流,简单易懂
>但是也可能出现关闭失败的现象,比如移动硬盘被拔了,磁盘满了

##### fprintf fscanf
```C
int fprintf (FILE* stream, const char*format, [argument])
int fscanf(FILE*stream,constchar*format,[argument...]);
```
##### rewind
```C
void rewind(FILE *stream);
```
将文件流指针移到开始


