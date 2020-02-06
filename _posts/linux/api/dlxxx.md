---
title: 动态库
date: 2020-02-03 14:54
categories: 
tags: 
---

关于链接相关的知识, 在csapp第七章有详细介绍。总的来说, 链接部分主要分成三部分内容
* 静态链接
* 编译期动态链接
* 运行时动态加载, 链接

dlxxx系列函数主要是在运行时动态加载, 链接

function | 意义
:-: | :-: 
dlopen | 加载, 链接共享库
dlclose | 卸载共享库
dlsym | 从打开的共享库获取莫哥symbol的地址
dlerror | 上面的dl函数出现问题, 通过调用dlerror会返回字符串描述具体信息

## dlerror
```c
#include <dlfcn.h>
char* dlerror()
```
这个函数会以字符串形式返回自从上一次调用**dlerror()**之后来自dlopen, dlsym, dlclose函数的错误信息, 如果没有发生错误, 或者已经调用过**dlerror()**,就会返回NULL

## dlopen
```c
#include <dlfcn.h>
void *dlopen(const char *filename, int flag)
```
### filename
filename的意义
* ~~NULL, 返回的句柄作用于主程序~~, 没懂啥意思, 哪个老哥给我解释一下
* 包含'/',表示传入的filename是个文件路径, 程序会去加载那个文件
* 非以上两种, 则当成一个动态库, 下面是动态库加载的逻辑

- elf文件, 如果带有DT_RPATH，且不包含DT_RUNPATH，就会搜索这个标记下面的文件
- 检查LD_LIBRARY_PATH环境变量的路径下是否包含目标动态库
- elf文件, 检查DT_RUNPATH是否包含目标文件
- 检查/etc/ld.so.cache是否包含filename
- 按照顺序搜索/lib /usr/lib

如果filename加载的对象依赖于其他动态库, 则该规则会自动得执行下去
#### NULL的情况
```c
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
typedef void (*bitch)();
void shit(){
    printf("shit");
}
int main(){
    void *handler = dlopen(NULL, RTLD_LAZY);
    bitch bitch1;
    char *error;
    if(!handler){
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    bitch1 = dlsym(handler, "shit");
    if((error = dlerror()) != NULL){
        fprintf(stderr, "sym %s\n", dlerror());
        exit(1);
    }

    (*bitch1)();
    return 0;

}
```

一开始编译的时候我没有加上**-rdynamic**选项, 导致找不到shit函数, 加上之后果断好了, 那么**-rdynamic**有什么作用呢
-rdynamic可以将本模块所有符号同步到.dynsym符号表中。
那么.dynsym有什么呢?

.dynsym用来保存与动态链接相关的导入导出符号, 可以通过readelf查看上面代码最后生成的elf内容
![动态符号表](/uploads/linux/api/dlxxx/dlxxx_dynsym.jpg)
![符号表](/uploads/linux/api/dlxxx/dlxxx_dynsym1.jpg)
嗯 elf格式还是不精通啊


### flag
幸运2选1
* RTLD_LAZY: 延迟绑定, 在执行他们的代码时解析符号, 如果没有用到过这个符号, 就不会被解析(ps, 只对函数有用, 变量符号一开始就会被绑定), glibc 2.1.1以后, 这个选项会被LD_BIND_NOW覆盖
* RTLD_NOW: 立即绑定

可选flag
* RTLD_GLOBAL: 这个共享库的符号会被用来解析后续加载的符号解析
* RTLD_LOCAL: 和上面的相反, 只用于本地
* RTLD_NODELETE: dlclose()的时候不会把这个动态库的变量去掉, 以防止下次打开需要重新加载
* RTLD_NOLOAD: 不加载该动态库, 那有什么用呢。1: 检查某个动态库是否已经被加载了, 如果没被加载返回NULL，被加载了返回handler 2:修改已经被记载的动态库的flag, 比方说原来已经设置成local的flag， 可以重新设置成global
* RTLD_DEEPBIND: 符号的查找范围优先使用自己的符号而不是全局符号



## dlsym
```c
#include <dlfcn.h>
void *dlsym(void* handler, const char* symbol)
```

这个函数以dlopen返回的句柄为第一个参数, 一个非空字符串为第二个参数表示需要查找的符号，查找在这个动态库中这个符号的位置。如果在这个动态库中, 以及伴随这个动态库一起被载入的动态库中没有找到相应的符号
，函数返回NULL(查找方法使用广度优先的遍历方法)。

由于存在确实符号不存在返回NULL的情况, 所以为了区分报错返回NULL还是确实是NULL，使用dlsym的正确姿势是, 先调用dlerror将错误信息清空, 然后调用dlsym， 在调用dlerror，通过判断最后的dlerror返回值确定
返回信息。

## dlclose
```c
#include <dlfcn.h>
int dlclose(void* handler)
```
dlclose使该动态库的引用数减1，如果引用数减到了0，那么这个动态库就会被卸载。
dlclose返回0表示成功close，非0表示卸载失败


# 备注
## DT_RPATH DT_RUNPATH
搜了一圈资料, 各种混乱, 反正名词意义上来说, 应该是建议使用DT_RPATH，那么gcc怎么设置DT_RPATH呢, **gcc --rpath**

