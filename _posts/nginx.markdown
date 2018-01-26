---
title: nginx
date: 2018-01-26 14:55
categories: 
tags: 
---
# 安装

# 配置文件
该部分学习内容摘自 agentzh的nginx教程系列,可以理解为阅读笔记。
[agentzh的教程](http://openresty.org/download/agentzh-nginx-tutorials-zhcn.html#00-Foreword01)

初次接触nginx最大的给我的感触就是,配置文件好特么麻烦！完全就是学一门新的语言= =
## 变量
在配置文件中,变量只能有一种类型,那就是字符串！

赋值方法:
```nginx
set $a "hello world"; #nginx_rewrite set命令方式赋值

geo $doller {
    default "$";
}                      #geo命令赋值

set_unescape_uri $name $arr_name;#设置name为$arr_name转码后的变量值,创建,赋值一条龙服务
```
[geo命令](###geo)

变量名必须带有$前缀(PHP程序员可能觉得没什么问题,但是Java,C++的筒子们可能觉得难受了),其实好处很简单:在字符串内的变量拼接变得很简单了
```nginx
location /test{
    set $a "hello";
    set $b "$a xx $a";

    echo $b;
}
```
注意在echo 时 $符是无法转义的,不要妄想\$可以完成转义,处理办法可以将$赋值给某个变量然后输出那个变量,通过geo方法,**通过set方法会失败的**,
比如
```nginx
set $a "$";#nginx: [emerg] invalid variable name报错!
```

```shell
curl Hostname/test
#ans
hello xx hello
```

如果变量解析时需要与其余字符串结合例如
```nginx
set $a "aa";
echo $ab;  #这个时候你就无法解析$a了
echo ${a}b;  #这个才是正确的姿势,以我PHP的角度看,肥肠顺眼
```
### 变量的赋值与创建
set,geo指令可以在给变量赋值之余还可以进行变量的创建,**nginx变量的创建发生在nginx加载配置文件的时候,而变量的赋值发生在实际请求的处理的时候**。
一个变量如果没有set 后者geo直接处理,则会造成nginx启动失败！

同时,nginx的变量可以理解为一个全局变量(对单次请求而言！！！),只要在某一个模块做了赋值,无需在其他模块再做创建操作,可以直接调用,修改,但是实际的值,还是依赖于<font color=red>**实际请求过程中的流程中数据的流转**</font>
```nginx
    server {
        listen 8080;

        location /foo {
            echo "foo = [$foo]";
        }

        location /bar {
            set $foo 32;
            echo "foo = [$foo]";
        }
    }
```
```shell
$ curl 'http://localhost:8080/foo'
foo = []

$ curl 'http://localhost:8080/bar'
foo = [32]

$ curl 'http://localhost:8080/foo'
foo = []
```

```nginx
    server {
        listen 8080;

        location /foo {
            set $a hello;
			echo_exec /bar
        }

        location /bar {
            echo "a = [$a]";
        }
    }
```
echo_exec可以实现location之间的跳转,rewrite规则也可以实现这个跳转

### 内建变量
最常见的内建变量是一些关于请求信息的变量
* $uri 当前请求uri(经过解码,不包含请求参数)
* $request_uri(原始uri)
* $arg_xxxx变量群(xxxx匹配各个各个请求的变量名,注意:**不区分大小写**,uri中的参数不解码!!,想要查看解码的参数？参见[set_unescape_uri](###set_unescape_uri))
* $cookie_xxxx变量群
* $http_xxxx变量群
* $send_http_xxxx变量群
详情请查阅ngx_http_core模块官方文档

很多内建变量都是只读的,注意以上变量轻易不要修改,很可能引起崩溃,当然也有支持修改的内建变量

* $args 当前请求的参数(注意$args修改后,$arg_xxx系列也会自己修改,这个参数经过[proxy_pass](###proxy_pass)会继续透传到下面的代理服务器)
* 




## 命令作用说明
### geo
该命令由ngx_http_geo_module提供支持,默认情况下nginx自动编译了该模块
该命令主要作用为根据某一个变量值,给另一个变量赋值
```nginx
geo [$address] $variable {}
```
$address为默认值,即默认情况下根据address进行匹配,也可以手动根据某一变量赋值
详细解释查看[geo网络解释](http://www.ttlsa.com/nginx/using-nginx-geo-method/)

### echo_exec
### rewrite
### set_unescape_uri
### proxy_pass

# 执行原理

# 拓展开发
## openresty



