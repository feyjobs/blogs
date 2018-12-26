---
title: nginx_source_conf_load
date: 2018-09-20 15:41
categories: 
tags: 
---

# 关键数据结构
核心数据结构
```c
struct ngx_command_s {
    ngx_str_t             name; //配置项名称
    ngx_uint_t            type; //配置项类型
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf); //配置项解析方法
    ngx_uint_t            conf; //配置项相对
    ngx_uint_t            offset;
    void                 *post;
};

struct ngx_conf_s {
    char                 *name;//当前解析的命令名
    ngx_array_t          *args;//当前命令的参数

    ngx_cycle_t          *cycle;
    ngx_pool_t           *pool;
    ngx_pool_t           *temp_pool;
    ngx_conf_file_t      *conf_file;//将要解析的配置文件
    ngx_log_t            *log;

    void                 *ctx; //指令上下文
    ngx_uint_t            module_type; //模块类型
    ngx_uint_t            cmd_type; //命令类型

    ngx_conf_handler_pt   handler;//自定义配置处理方法
    void                 *handler_conf; //自定义配置处理方法所需参数
};

typedef struct {
    ngx_file_t            file;       //文件属性
    ngx_buf_t            *buffer;     //文件内容
    ngx_buf_t            *dump;       
    ngx_uint_t            line;       //文件行数
} ngx_conf_file_t;

```

# 关键函数
## ngx_conf_param
## ngx_conf_parse
prev的作用是什么呢,我猜是暂时保存当前的环境,从而提供一个在递归调用文件解析的时候的上下文环境