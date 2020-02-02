---
title: 内存那些事
date:  2019-1-16 11:21:38
categories: 
tags: 
---

## 内存出现的问题
### free报错
事故现场
```c
#include <stdlib.h>
int
main(){
    void* ptr = malloc(2014);
    ptr = ptr + 3;
    free(ptr);
    return;
}
```
这段代码先申请一段2014 bytes的空间,然后free +3地址
错误信息
![-name](/uploads/C/c_memory.png)
***************************************************************************************
类似的
```c
#include <stdlib.h>
int
main(){
    int a = 3;
    int* ptr = &a;
    free(ptr);
    return;
}
```
直接free了一个栈地址,也会直接报错
![-name](/uploads/C/c_memory_free_stack.png)
总的来说,都是**invalid pointer**
****************************************************************************************
在测试中,还发现了了其他错误
```c
#include <stdlib.h>
int
main(){
    void* ptr = malloc(2014);
    free(++ptr);
    return;
}
```
![-name](/uploads/C/c_memory_free_munmap.png)
具体原因先参考

***************************************************************************************





### malloc报错
在某一次使用堆时,使用了过量内存,导致了这个内存鼓掌,malloc都不让了= =,后续再看下
![-name](/uploads/C/c_memory_malloc.png)
