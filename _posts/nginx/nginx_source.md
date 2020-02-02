---
title: nginx源码学习
date: 2018-05-31 11:31
categories: 
tags: 
---

# 编译篇
本篇旨在说明解析nginx编译脚本的运行过程
## 开源版

## BROC

# code
通用各个模块
* init_master 暂时无用
* init_module
main->ngx_init_cycle->ngx_init_cycle->init_module

* init_process(fork之后在各自子进程)
main->ngx_master_process_cycle->ngx_start_worker_processes->ngx_spawn_process(fork)->ngx_worker_process_cycle->ngx_worker_process_init

* init_thread 暂时无用
* exit_thread 暂时无用
* exit_process
main->ngx_master_process_cycle->ngx_start_worker_processes->ngx_spawn_process(fork)->ngx_worker_process_cycle->ngx_worker_process_exit
* exit_master
main->ngx_master_process_cycle->ngx_master_process_exit


## HTTP模块
main->ngx_init_cycle->ngx_conf_parse->ngx_conf_handler->ngx_http_block(http command set function)
* create_main_conf
* create_srv_conf
* create_loc_conf
* preconfiguration
* init_main_conf
* merge_srv_conf
* merge_loc_conf
* postconfiguration

连接事件框架
ngx_http_block->ngx_http_optimize_servers->ngx_http_init_listening->ngx_http_add_listening->ngx_http_init_connection

## 通用数据结构
### ngx_pool_t
#### 主结构
```c
struct ngx_pool_s {
    ngx_pool_data_t       d;
    size_t                max;
    ngx_pool_t           *current;
    ngx_chain_t          *chain;
    ngx_pool_large_t     *large;
    ngx_pool_cleanup_t   *cleanup;
    ngx_log_t            *log;
};

typedef struct {
    u_char               *last;
    u_char               *end;
    ngx_pool_t           *next;
    ngx_uint_t            failed;
} ngx_pool_data_t;

struct ngx_pool_large_s {
    ngx_pool_large_t     *next;
    void                 *alloc;
};
```

###
