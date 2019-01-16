---
title: nginx_shm
date: 2018-12-19 11:07
categories: 
tags: 
- nginx
---
共享内存是nginx进程间通信的重要机制,本文讲从共享内存的使用与原理进行剖析。

# 使用
nginx提供给模块开发者一些共享内存api
## 数据结构
```c
struct ngx_shm_zone_s {
    void                     *data;	//共享内存初始化传参
    ngx_shm_t                 shm;	//共享内存信息
    ngx_shm_zone_init_pt      init;	//共享内存初始化函数
    void                     *tag;	//防冲突标签，一般是模块名
    void                     *sync;
    ngx_uint_t                noreuse;  /* unsigned  noreuse:1; */ //是否可复用
};

typedef struct {
    u_char      *addr;	//共享内存地址
    size_t       size;	//共享内存大小
    ngx_str_t    name;	//共享内存名
    ngx_log_t   *log;	//共享内存操作期间日志处理方法
    ngx_uint_t   exists;   /* unsigned  exists:1;  */  
} ngx_shm_t;
```
<p>
<font color="green">ngx_shm_zone_s</font>用于在nginx cycle中记录某一个共享内存块
</p>
<p>
<font color="green">ngx_shm_t</font>用来记录共享内存块的信息
</p>

## api
### 添加共享内存
> ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name, size_t size, void *tag)

param:
 - ngx_conf_t *cf  配置文件环境
 - ngx_str_t *name  共享内存名
 - size_t size  共享内存大小
 - void *tag    共享内存去重标签

return: ngx_shm_zone_t*


# 原理
![shared memory](/uploads/nginx/nginx共享内存.png)

## slab内存管理机制
内存分配机制:
* best-fit 遍历所有内存块区块,找到<font color="red">最合适</font>的区块进行分配 慢-但是碎片少
* first-fit 遍历空闲内存块列表,找到<font color="red">第一块</font>适合区块进行分配 快-但是碎片多

slab机制:best-fit
