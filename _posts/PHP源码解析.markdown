---
title: PHP源码解析
date: 2017-12-18 16:08:10
categories: 
- Code
- PHP
- SourceCode
---

# 基本调试命令
```shell
/opt/compiler/gcc-4.8.2/bin/gdb php/bin/php-cgi
set solib-search-path php/ext:lib/gcc-4.8.2
r -c /home/map/Documents/wodp_debug/php/etc/php.ini -p /home/map/Documents/wodp_debug/php --fpm-config /home/map/Documents/wodp_debug/php/etc/php-fpm.conf
```
#工具篇

