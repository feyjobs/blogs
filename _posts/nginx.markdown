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
* $arg_xxxx变量群(xxxx匹配各个各个请求的变量名,注意:**不区分大小写**,uri中的参数不解码!!,想要查看解码的参数？参见[set_unescape_uri](###set_unescape_uri),<font color=red>不知道为什么如果参数是&的形式传进去的答应不出来这个参数需要后续研究</font>)
* $cookie_xxxx变量群
* $http_xxxx变量群
* $send_http_xxxx变量群
详情请查阅ngx_http_core模块官方文档

很多内建变量都是只读的,注意以上变量轻易不要修改,很可能引起崩溃,当然也有支持修改的内建变量

* $args 当前请求的参数(注意$args修改后,$arg_xxx系列也会自己修改,这个参数经过[proxy_pass](###proxy_pass)会继续透传到下面的代理服务器,$arg_xxxx系列变量其实是通过时事获取的方式来)

### 变量读取方式
#### 取处理程序
nginx模块可以自动为其创建的变量使用**值容器**,作为其取处理程序的计算结果的缓存,nginx_map在处理过程中,默认认为取处理程序的结果十分重要,所以默认进行缓存.
case:
```nginx
map $args $foo{
    default 0;
    debug   1;
}

server {
    listen 8080;

    location /test{
        set $orig_foo $foo;
        set $args debug;

        echo "orig: $orig_foo";
        echo "foo: $foo"
    }
}
```
运行访问localhost:8080/test 输出 0 0
访问/test?debug 输出1 1
说明map指令确实对所有的映射关系做了缓存

### 请求
前面我们说过,nginx变量的值与当前请求相关.但是请求究竟具体意义没有说明,在nginx世界,请求包括主请求和子请求
### 主请求
主请求就是从nginx外部发起,通过http客户端发起的请求

### 子请求
由nginx正在处理的请求发起的内部级联请求。变面上很想http请求,但是实际上和http一点关系没有,只不过是nginx的一种内部的抽象调用,只不过是方便nginx将主请求分解为各个子请求,并行或者串行完成主请求
case:
```nginx
location /main {
    echo_location /bar;
    echo_location /foo;
}

location /foo {
    echo fool;
}
location /bar {
    echo bar;
}
```
依次调用了/foo /bar 两个location
在回到之前关于请求与变量值之间的关系问题,现在我们要谈讨下,子请求的值与主请求的相关性.

```nginx
location /main {
    set $var main;
    echo_location /bar;
    echo_location /foo;
    echo $var;
}

location /bar{
    echo bar;
    echo $var;
}
location /foo{
    echo foo;
    echo $var;
}
```
可以看到在子请求中的$var的值都是空的,没有进行赋值操作。

#### 父子请求共享变量
然而世间万物就是这么麻烦,有一些命令吊起子请求,居然能共享变量,比如说nginx_auth_request的auth_request
```nginx
location /main{
    set $var main;
    auth_request /sub;
    echo "main $var;"
}

location /sub {
    set $var sub;
    echo "sub $var";
}
```
很好果不其然的输出了, main sub
而且在子请求还完全不输出= =
为毛,因为auth_request完全ignore子请求的结果!只要返回了20x就开开心心往下跑了!
如果不是20x就直接跳到错误页

#### 只管主请求的变量
好了上面我们说了有一些子请求的发起方式会造成,父子请求共享变量,接下来我们要介绍一些变量,不管是不是父子请求,他都只管主请求.
##### request_method
$request_method描述的是请求的方法,他只会存储主请求的请求方法
```nginx
location /main {
    echo "main $request_method";
    echo_location /sub;
}

location /sub {
    echo "sub $request_method";
}
```
无论怎么改变请求方法,两个$request_method打印的都是主请求的方法,同时需要注意的是,缓存所依赖的变量容器是与当前请求绑定的,父子请求不共享变量.
所以$request_method的变量始终存储的是主请求的变量
那么如何获取当前请求的请求方法呢, **$echo_request_method**这个变量记录当前请求的请求方法
##### request_uri

### invalid & missing
前面说过Nginx的变量只有一种格式就是字符串,但是变量可能不存在也可能没有意义,那么如何区分这两种状态呢!
下一节所描述的两个关键词:
* invalid 非法:指的是创建了该变量,但是并没有给他正确的赋值
* missing:指的是没有创建这个变量,但是直接获取该变量

通过第三方模块ngx_lua,我们可以在lua代码中获取这两个值
```nginx
location /main {
    content_by_lua '
        if ngx.var.arg_name == nil then
            ngx.say("name: missing")
        else 
            ngx.say("name [",ngx.var.arg_name,"]")
        end
    ';
}
```
通过执行访问控制变量的传入可以输出不同的结果,判断该值为valid还是missing
通过ngx_lua模块的功能我们区分出了,不同的nginx的值的的功能,<font color=red>这个实现的技巧的关键是lua在读取nginx变量如果没有创建的话会初始化为nil</font>




## 模块作用说明
### ngx_http_geo_module
#### geo
该命令由ngx_http_geo_module提供支持,默认情况下nginx自动编译了该模块
该命令主要作用为根据某一个变量值,给另一个变量赋值
```nginx
geo [$address] $variable {}
```
$address为默认值,即默认情况下根据address进行匹配,也可以手动根据某一变量赋值
详细解释查看[geo网络解释](http://www.ttlsa.com/nginx/using-nginx-geo-method/)

### ngx_echo
#### echo_exec
### ngx_rewrite
#### rewrite
### ngx_set_misc
#### set_unescape_uri
### ngx_proxy
#### proxy_pass
### ngx_map
#### map
描述一个映射关系
```nginx
map $args $foo{
    default 1;
    hello   2;
}
```
如上所示,表示根据$args来判断$foo的值,如果$args不存在,则$foo为1,其余情况则是,更具$args的值,对$foo进行映射
map指令的执行时机,map指令和server一个级别,所以会有小伙伴担心会不会所有location都会被执行这个指令,其实不时的,map指令本质上是为用户变量注册取处理程序,实际的映射关系也是在取处理程序中维护的,然而取处理程序只会在用户读取的 时候 才会精心调用,所以这个指令只会在用户执行读取变量时才会执行
这种数据的处理方法称之为**惰性求值**,Haskell也使用了这种思想.

### ngx_lua
将lua解释器嵌入nginx核心中,lua代码可以直接嵌在nginx配置文件中,也可以放置在外部的.lua文件中,然后在Nginx配置文件中引用这些路径
lua代码中引用nginx变量的方法,ngx.var.xxxxx
例如nginx有一个变量$arg_name
则在lua中引用该变量的方法 ngx.var.arg_name


# 执行原理

# 拓展开发
## openresty



