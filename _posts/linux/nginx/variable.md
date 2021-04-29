---
title: variable
date: 2020-04-08 15:08
categories: 
tags: 
---

# 变量
nginx有两种变量, 一种set实时设置的变量, 一种模块内自定义变量

```c
//根据name查找配置文件中的变量index, 如果不存在则插入变量列表, 并返回index
ngx_http_get_variable_index(ngx_conf_t *cf, ngx_str_t *name)
{
    ngx_uint_t                  i;
    ngx_http_variable_t        *v;
    ngx_http_core_main_conf_t  *cmcf;

    if (name->len == 0) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid variable name \"$\"");
        return NGX_ERROR;
    }

    //获取http core module main配置
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    v = cmcf->variables.elts;

    // 判断变量列表是否已经初始化, 如果没有则初始化
    if (v == NULL) {
        if (ngx_array_init(&cmcf->variables, cf->pool, 4,
                           sizeof(ngx_http_variable_t))
            != NGX_OK)
        {
            return NGX_ERROR;
        }

    } else {
        // 已经初始化, 则遍历列表, 寻找匹配的变量
        for (i = 0; i < cmcf->variables.nelts; i++) {
            if (name->len != v[i].name.len
                || ngx_strncasecmp(name->data, v[i].name.data, name->len) != 0)
            {
                continue;
            }

            return i;
        }
    }

    //没有找到对应的变量, 说明列表中不存在, 则将该变量插入列表
    v = ngx_array_push(&cmcf->variables);
    if (v == NULL) {
        return NGX_ERROR;
    }

    v->name.len = name->len;
    v->name.data = ngx_pnalloc(cf->pool, name->len);
    if (v->name.data == NULL) {
        return NGX_ERROR;
    }

    ngx_strlow(v->name.data, name->data, name->len);

    v->set_handler = NULL;
    v->get_handler = NULL;
    v->data = 0;
    v->flags = 0;
    v->index = cmcf->variables.nelts - 1;

    return v->index;
}
```

# 模块预设变量

