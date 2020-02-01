---
title: Linux IO
date: 2019-7-18 19:56:38
categories: 
- Linux
- IO
tags:
---

# 基础IO
## Open
```c
#include <fcntl.h>
int open(const char *path, int oflag, .../*mode_t mode*/); 
int openat(int fd, const char *path, int oflag, .../*mode_t mode*/); 
```
| flags   |      meaning      |
|----------|:-------------:|
| O_RDONLY | 只读  |
| O_WRONLY | 只写  |
| O_RDWR |  读写|
| O_EXEC |  执行|
| O_SEARCH |  搜索|

* *O_EXEC*在linux里不存在,设计初衷应该是,防止一些文件权限格式是111的,即只有执行权限,然后可以在用fexecve函数时,打开这个文件
* *O_SEARCH*在linux不存在,设计初衷可能是遍历文件夹下文件列表
* 以上为必传字段!

| flags   |      meaning      |
|----------|:-------------:|
| O_APPEND | 追加写  |
| O_CLOEXEC | 只写  |
| O_CREAT|  创建,如果不存在|
| O_DIRECTORY|  路径必须是目录,否则报错|
| O_EXCL | 如果存在报错| 
| O_NOFOLLOW| path不准是个软连接| 
| O_NONBLOCK| 管道文件,块文件,字符文件打开时不再阻塞| 
| O_SYNC| 强制刷新到磁盘| 
| O_TRUNC| 打开文件,如果已经有内容,清除| 
| O_DSYNC| 写入数据,在属性数据更新前返回,O_SYNC在属性更新后返回| 
| O_RSYNC| 等待同一部分写入完成后,才读取成功| 
| O_NOCTTY | *todo*| 
| O_TTY_INIT | *todo*| 
| O_TTY_INIT | *todo*| 

### xxx && xxxat()
这两个系列函数在linux中非常常见
* path如果传的是个绝对路径,那么fd被忽略,相当于调用open
* path如果传的是个相对路径,fd是一个目录fd，则会根据fd对应的目录确定文件路径
* 如果fd设置了*AT_FDCWD*,那么和调用open一致

那么这类函数设置了有什么意义呢主要是有两个
* 多线程编程的时候进程的当前路径是所有现成共享的,有了这个可以现成创建后创建一个线程的目录,该线程的操作都在这个目录下完成,这样就不会有冲突
* TOCTTOU问题,这个问题是说一些恶意程序,在逻辑上应该是原子新的操作中,插入了恶意代码,从而造成不良影响
  举个例子,在open文件之前,恶意代码修改当前文件目录到一个敏感地方(chdir),之后的open操作就会在那个目录下进行,然后openat函数指定了打开文件对应的文件目录,所以不会有这个问题


## creat
```c
#include <fcntl.h>
int creat(const char* path, mode_t mode);
```
垃圾函数,被淘汰，等同于
```c
open(path, O_CREAT|O_WRONLY|O_TRUNC,mode);
```

## close
```c
#include <unistd.h>
int close(int fd);
```
关闭文件,并且**释放进程加在文件上的记录锁**

## lseek
```c
#incude <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
```
* SEEK_SET 文件开始偏移
* SEEK_CUR 当前偏移开始偏移
* SEEK_END 文件末尾偏移
设置文件当前偏移量,
**如果文件以O_APPEND打开,write之后依然会追加到文件末尾**


## read
```c
#include <unistd.h>
ssize_t read(int fd, char *buf, size_t len)
```
返回读到的字节数,如果到了文件末尾EOF返回0，读文件末尾需要单独一次read，即剩余3byte,读4byte,第一次read返回3,第二次read返回0

读各种io设备都有可能造成返回的res != len
* 终端 todo
* 套接字 todo
* fifo pip todo
* 信号中断情况 todo
  
## write
```c
#include <unistd.h>
ssize_t read(int fd, char *buf, size_t len)
```
写入文件,异常状况
* 磁盘写满
* 写入量超过进程文件写入量上限

## dup&&dup2
```c
#include <unistd.h>
int dup(int fd);
int dup2(int fd, int fd2);
```
这两个是重定向函数,作用都是创建一个新的文件描述符fd，指向传入参数的fd所对应的文件表项,那为什么要两个呢
* dup返回一个当前文件描述符最新的一个未使用描述符
* dup2使fd2指向fd的文件描述符,如果fd2已经打开,就会先关闭再打开

**dup2**的关闭,打开具有原子性,所以相比fcntl的重定向更加优秀

## sync && fsync && fdatasync
```c
#include <unistd.h>
int fsync(int fd);
int fdatasync(int fd);
void sync(void);
```
这几个函数都和缓冲区刷新到磁盘相关,简单介绍下磁盘写入过程,调用write后会先写入内核缓冲区,缓冲区定期会刷新带写入队列,然后再写入到磁盘

* fsync会将当前fd的缓冲区内容写入磁盘,直到写入完成返回
* fdatasync将当前缓冲区内容写入磁盘,但是不等更新文件信息(长度 offset等)就返回
* sync讲当前缓冲区全部扔到写入队列,然后返回

## fcntl
重头戏中的重头戏,这个函数给我感觉演杂技呢,这么多功能
```c
#include <fcntl.h>
int fcntl(int fd, int cmd, .../*other args*/);
```
### 复制类
* F_DUPFD 创建fd(新fd为大于等于第三个参数的最小fd)指向第一个参数指向的文件表项,但是描述符标志位是自己的,所以就有了下面的选项
* F_DUPFD_CLOEXEC F_DUPFD的文件描述符创建之后,在exec后fd依然保留,这个选项不保留

### 标志位类
* F_SETFD
* F_GETFD
这两个是读写文件描述符标志位的功能,目前只支持设置O_CLOEXEC这样的功能,即设置exech后fd是否还保持

### 文件状态标志
* F_GETFL
* F_SETFL
文件状态标志位获取,嗯 就是open的时候那一堆参数
#### 访问权限标志位
* O_RDONLY 
* O_WRONLY 
* O_RDWR   
* O_EXEC   
* O_SEARCH 
这几个标志位不能通过**F_SETFL**进行修改
读取的时候,这几个标志位不能直接 & 来判断是否设置了该标志位,因为这几个标志位非常傻逼得没有设置一个取一位,造成 & 的时候O_RDWR校验O_RDONLY O_WRONLY都能通过,
所以正确的姿势是
```c
val = fcntl(fd, F_GETFD, 0);
switch (val & O_ACCMODE){
    case O_RDONLY:
    xxx
}
```
先用O_ACCMODE去除其他标志位,再一个个比较

开启某个标志位,先获取当前标志位
```c
val = fcntl(fd, F_GETFD, 0);
newval = val | O_XXXXX;/*开*/
newval = val & ~O_XXXXX;/*关*/
```

**O_SYNC**在linux系统中无法通过fcntl设置,没错,返回成功也没设置上,不信那下面的代码试试
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int fd = open("fcntltxt",O_RDWR);
    int val =  fcntl(fd, F_GETFL, 0);
    val |= O_SYNC;
    int res = fcntl(fd, F_SETFL, val);
    perror(NULL);
    printf("%d", res);
    val = fcntl(fd, F_GETFL, 0);
    if(val & O_SYNC){
        perror("has");
    }

    int fd2 = open("aa",O_RDWR|O_SYNC);
    val = fcntl(fd2, F_GETFL, 0);
    if(val & O_SYNC){
        perror("has2");
    }

    return 0;
}
```
可以看到open带上O_SYNC是有该标志位的,而fcntl是没有的

### 信号处理类(todo)
* F_GETOWN
* F_SETOWN
  
### 文件锁类(todo)
* F_SETLK
* F_GETLK
* F_SETLKW 


## ioctl
终端io经常用该函数操作,对一些其他io设备进行特殊操作,先不展开(todo)
```c
#include <sys/ioctl.h>
int ioctl(int fd, int request,...);
```

# 文件与目录
主要进行文件和目录的属性操作
## 基本结构
```c
struct stat{
    mode_t st_mode; /*权限标志位*/
    ino_t  st_ino; /*inode编号*/
    dev_t  st_dev; /*设备号*/
    dev_t  st_rdev;/*特殊设备设备号*/
    nlink_t st_nlink; /*硬链接数*/
    off_t     st_size;        /* 大小 */
    blksize_t st_blksize;     /* 操作系统单个磁盘块大小 */
    blkcnt_t  st_blocks;      /* 磁盘块数 */

    struct timespec st_atim;  /* 最后一次文件内容被访问时间 */
    struct timespec st_mtim;  /* 最后一次文件内容被修改时间 */
    struct timespec st_ctim;  /* 最后一次被修改时间 (除了内容还包括权限,owner等属性)*/

    uid_t     st_uid;         /* User ID of owner */
    gid_t     st_gid;         /* Group ID of owner */
}
```

## 获取函数
```c
#include <ys/stat.h>
int stat(const char *restrict path, struct stat *restrict buf);
int fstat(int fd, struct stat *restrict buf);
int lstat(const char *restrict path, struct stat *restrict buf);
int fstatat(int fd, const char *restrict path, struct stat *restrict buf, int flag);
```
* stat 根据绝对路径获取文件信息
* fstat 根据fd获取文件信息
* lstat 如果文件是一个软连接,直接返回软连接的文件信息,而不会往下继续追到原始文件
* fstatat 和其他at函数一样,注意如果flag设置了**AT_SYMLINK_NOFOLLOW**,则该函数不会继续往下,而是返回软连接

## 文件类型
文件类型信息有几个宏提供主要用于判断st_mode
| type|   macro|
|----------|:-------------:|
| 文件| S_ISREG  |
| 目录| S_ISDIR  |
| 字符特殊文件|  S_ISCHR|
| 块特殊文件 |  S_ISBLK|
| 管道| S_ISFIFO |
| 软链| S_ISLNK |
| 套接字| S_ISSOCK |

进程间通信对象可以描述为文件,故也有几个宏来描述这个功能,处理对象为stat
| type|   macro|
|----------|:-------------:|
| 消息队列| S_TYPEISMQ  |
| 信号量| S_TYPEISSEM|
| 共享内存| S_TYPEISSHM|

## 账号
|用户|意义|
|----------|:-------------:|
| 实际用户id/组id| 登陆用户的信息  |
| 有效用户id/组id/附属组id| 文件访问权限ID|
| 保存的用户ID/组ID| 由exec保存|

**设置用户id**
在st_mod中设置了该标志位后,在**执行**该文件时,将进程的有效用户id/组id设置成文件的所有id

| type|   变量|
|----------|:-------------:|
| 设置用户id| S_ISUID|
| 设置组id| S_ISGID  |
```c
S_ISUID & stat.st_mode
```
判断方法如上

## 文件访问权限

|st_mode屏蔽|  意义 |
|----------|:-------------:|
|S_IRUSR|用户读|
|S_IWUSR|用户写|
|S_IXUSR|用户执行|
|S_IRGRP|组读|
|S_IWGRP|组写|
|S_IXGRP|组执行|
|S_IROTH|其他读|
|S_IWOTH|其他写|
|S_IXOTH|其他执行|

权限规则：
* 我们用绝对路径打开任何一个文件,都需要路径上每个目录的执行权限
* 文件读权限对应O_RDONLY|O_RDWR
* 文件写权限对应O_WRONLY|O_RDWR
* O_TRUNC打开文件需要文案写权限
* 文件夹中创建新文件需要**写**+**执行**权限
* 删除一个文件,需要对文件夹有**写**+**执行**权限
* exec函数需要有该文件的执行权限

## 新文件与文件夹所有权
* 新文件的owner为进程的有效用户id
新文件的组id可以是以下两种之一
* 有效组id
* 所在文件夹组id

经测试,linux下新文件的所属组id为进程的有效组id

## access && faccessat
这两函数是干啥的:
用于检测当前进程的实际用户id是否有对某一文件的权限
比方说某个进程设置了 设置用户id位 则在exec之后,进程的有效用户id会变成该文件的所有者用户id但是该程序想check一下进程的实际用户id是否有某文件的读写权限 则需要该函数
```c
#include <unistd.h>
int access(const char *pathname, int mode);
int faccessat(nt fd, const char *pathname, int mode);
```
|flag|  意义 |
|----------|:-------------:|
|F_OK|文件是否存在|
|R_OK|是否有读权限|
|W_OK|是否有写权限|
|X_OK|是否有执行权限|

## umask
umask设置了一个文件创建的时候的屏蔽字,什么意思呢,就是创建的时候,umask包含的权限位,都会被干掉
```c
#include <sys/stat.h>
mode_t umask(mode_t mode);
```
设置了mode的位,创建的时候都会被屏蔽掉

## chmod && fchmod && fchmodat
更改文件访问权限函数
```c
#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int fd, const char *pathname, mode_t mode, int flag);a/*flag位AT_SYMLINK_NOFOLLOW则修改链接文件,不向下继续追到源文件 todo带测试*/
```

该函数执行前提:
* 进程有效id等于文件所有id
* root账号

可以修改的标志位:
* 用户读写执行
* 组读写执行
* 其他读写执行
* 设置用户id
* 设置组id
* 粘着位


## 粘着位
粘着位有什么用,首先我们回顾一个文件夹的操作权限问题
如果我们要在一个文件夹内创建新文件,我们必须要有w,x权限,但是如果other有了w+x权限,任何用户都可以在这个文件夹内翻江倒海,我们希望又一个限制,任何用户都可以在这个文件夹内创建,但是别人的文件你没有权限删除,于是有了粘着位
在粘着位内删除一个文件
* 你是root
* 你是相应文件夹的owner
可以参考/tmp文件夹,你可以随意在tmp中创建自己的文件,但是不能删除其他账户的文件

## chown && fchown && fchownat && lchown
```c
#include <unistd.h>
int chown(const char *path, uid_t uid, gid_t gid);
int fchown(int fd, uid_t uid, gid_t gid);
int fchownat(int fd,const char *path, uid_t uid, gid_t gid,int flag);
int lchown(const char *path, uid_t uid, gid_t gid);
```
和linux命令差不多意思,其他也和各种fxxxxat函数,lxxxxx函数差不多意思

## 文件长度
stat结构体包含,size,st_blksize,st_blocks三个字段与文件长度相关
* st_blksize 表示适合读写的区块大小,一般是4k
* st_blocks 表示分配了多少个512byte区块数
* size表示实际的文件大小

### 空洞问题
使用lseek设置偏移写入之后,可能偏移量大于当前文件的长度,造成了空洞,空洞不会算到实际大小中,但是会占用分配的区块数.
总体规则是
* ls 返回的是文件的逻辑大小,包含空洞
* stat 返回的block是实际的512块数量不包含空洞
* du 返回的是逻辑的1024块数量,包含空洞
  
### 文件截断
```c
#include <unistd.h>
int truncate(const char* pathname, off_t length);
int ftruncate(int fd, off_t length);
```
讲文件截断为一个length长度的文件,特殊的用O_TRUNCd打开文件,可以将文件截断为0，
如果文件长度小于length,可能创造一个空洞,也可能用\0补充

## link flinkat unlink unlinkat remove


