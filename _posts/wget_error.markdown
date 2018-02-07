---
title: wget_error
date: 2018-02-07 16:43
categories: 
- linux
- wget
- curl
tags: 
---
今天准备下载一个openresty源码玩玩,然而wget一直报错
![wget_error](/uploads/wget_error.png)
最后用
```shell
curl https://openresty.org/download/openresty-1.13.6.1.tar.gz > openresty-1.13.6.1.tar.gz
```
搞定了,查了下网上的说法,解释说默写网站不支持非浏览器的方法去访问,所以wget跪了,更加简练的方法是
```shell
curl -O  https://openresty.org/download/openresty-1.13.6.1.tar.gz
```
-O选项使用URL中默认的文件名保存文件到本地
