---
title: ffi
date: 2020-02-06 10:51
categories: 
tags: 
---

ffi作为luajit的核心战斗力(另一个是jit)在openresty项目中有广泛使用, 小小得研究一下

## lua调用c
问题的起源, 首先要从lua调用c函数开始说起,在lua官方文档中， lua语言调用c函数一般使用如下方法进行

```c
#include <lua.h>
#include <lauxlib.h>
#include <luaconf.h>
#include <lualib.h>
#include <stdio.h>
#include <dirent.h>

static int foo(lua_State *L) {
    const char *path = lua_tostring(L, 1);
    int pos = 1;
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(path);
    lua_newtable(L);
    while((ptr = readdir(dir)) != NULL){
        lua_pushnumber(L, pos);
        lua_pushstring(L, ptr->d_name);
        lua_settable(L, -3);
        pos++;
    }

    return 1;
}

int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, foo);
    lua_setglobal(L, "foo");
    luaL_loadfile(L, "hello.lua");
    lua_pcall(L, 0,0,0);
    return 0;
}
```

```lua
ans = foo("/root/code/git/Lua-5.1.4-codedump/test")
print(ans[10])
```
上面的代码中,首先启动lua虚拟机, 然后定义了lua需要调用的c函数,
注意这种方式lua调用的c函数有严格的格式
```c
typedef int (*lua_CFunction) (lua_State *L);
```
他必须接受一个lua_State作为输入, 然后返回一个int值, 这个int值表示这个c函数压到lua栈上的返回值个数,
可以看到foo函数读取了一个文件路径, 然后返回了这个文件路径下所有的文件
在lua调用这个c函数的时候, 传入一个路径， 并尝试读取了第十个文件, 以上就是原生的lua调用c函数的方法

## ffi
### 调用c函数
首先看一个[luajit提供的起始demo](http://luajit.org/ext_ffi.html)
```lua
local ffi = require("ffi")

ffi.cdef[[
int printf(const char *fmt, ...);
]]
ffi.C.printf("Hello %s!", "world")
```

一个非常简单的hello，world程序，主要流程如下
* require ffi库, 所以ffi是以一个lua库的形式提供的
* 调用cdef 后面跟的是一个字符串, 字符串里面包c的函数, 结构体定义.(ps: 我也是惊了lua函数调用不需要()也就算了, 如果是字符串连空格都不需要)
* 通过ffi.C.xxx调用cdef定义的c函数结构体

现在hello world是不是觉得不能满足我们了, 我们整把大的!
```lua
local ffi = require("ffi")

ffi.cdef [[
   int rmdir(const char* pathname);
]];
ffi.C.rmdir("byebye")
```
老样子, 这次我们调用gnu的redir函数, 结果也可以正常运行。

但是有一个问题, 现在我们调用的函数都是要么是c标准库, 要么是glibc提供的标准库, 肯定都是luajit编译的时候编译进去的
我不服! 我要调用自己写的服务!

先等等。 一会儿再说这个。

至少到目前为止, 我们可以看到要在lua中调用c函数, 用ffi的方式调用c函数有非常大的便捷度优势。

如果要用lua推荐的库类的方式来调用c函数怎么做呢 
```c
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int myadd(lua_State *L){
    int a = luaL_checknumber(L, 1);
    int b = luaL_checknumber(L, 2);
    lua_pushnumber(L, a+b);
    return 1;
}

static const struct luaL_Reg mylib[] = {
    {"add", myadd},
    {NULL, NULL}
};

int luaopen_mylib(lua_State *L){
    const char *const LIBRARY_NAME = "mylib";
    luaL_register(L, LIBRARY_NAME, mylib);
    return 1;
}
```
看上面要怎么做呢
* 创建一个额外的c文件
* 用myadd函数接受lua传递的参数, 并且校验
* 如果更加严谨和增加复用, 你会把a+b的部分单独写成一个函数
* 然后添加一个luaopen_xxxx的函数, 提供给require扫描
* 最后你才可以在lua调用这个库

是不是效果拔群?

### 调用非系统函数
上面的例子是lua函数调用系统|标准库函数,  那么lua怎么调用我们自己写的库函数呢
```c
#include <stdio.h>

void foo1(const char* str) {
    printf("%s", str);
};
```
上面我们写了一个非常普通的c语言函数, 把它编译成动态库
```shell
gcc -fPIC -shared foo.c -o foo.so       #差不都就这个意思, 可能差几个选项
```
那么怎么使用这个so呢
```lua
local ffi = require("ffi")
ffi.cdef [[
    void foo1(const char* str);
]]
foo = ffi.load("/root/code/git/LuaJIT-2.0.5/run/test/foo.so")
foo.foo1("caonima");
```
其他选项都平平无奇, 唯独出现了**ffi.load**, 这个函数能加载一个动态库, 可以按照库的名字来载入, 也可以直接传入一个路径来载入，嗯
怎么看都感觉和dlopen好像, 详情请参考{% post_link  linux/api/dlxxx dl系列%}, 总的来说非常明显了
* 系统函数: ffi.C.xxxxx
* 自定义库: xxx = ffi.load(xx)  xx.xxxxxx()
就这个意思吧。那么问题来了！如果要用一起奇怪的数据结构, lua不支持咋办, 还怎么愉快得和c语言交互。

新机子哇一滋莫嘿都滋.jpg

### 调用c结构体
ffi允许你创建和获取c结构体, 当然 这个主要是用来配合函数一起使用的, 但是单独使用也阔以
废话少说直接上代码
```c
#include <stdio.h>
#include <stdlib.h>

typedef struct A{
    int a;
    int b;
}a_t;

void foo1(const char* str) {
    printf("%s", str);
};

a_t foo2(int a,int b){
   a_t * foo = (a_t*)malloc(sizeof(a_t));
   foo->a = a;
   foo->b = b;
   return *foo;
};
```
编译成动态库foo.so
然后上lua代码
```lua
local ffi = require("ffi")
ffi.cdef [[
    typedef struct A {
        int a;
        int b;
    }a_t;
    void foo1(const char* str);
    a_t foo2(int a, int b);
]]
local foo = ffi.load("/root/code/git/LuaJIT-2.0.5/run/test/foo.so")
foo.foo1("caonima");
local s=foo.foo2(12, 13);
print(s.a)
```
运行结果
```shell
caonima12
```
好的, 所以说通过在ffi中声明结构体是可以在lua中直接使用的就像一个table一样

最后贴上
[ffi官方文档](http://luajit.org/ext_ffi.html)
