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

## 配置指令的执行顺序
nginx处理每一个用户请求时,都是按照若干个不同阶段依次处理的
例如
```nginx
location /test{
    set $a 32;
    echo $a;
    set $a 64;
    echo $a;
}
```
访问该请求,我们看到了结果
64
64
Why!
因为nginx指令是依赖**执行阶段**进行执行,指令的执行顺序并不依赖于配置项的先后顺序,nginx一共有11项执行顺序,最主要的几个执行阶段依次有rewrite,access,content阶段,
echo指令在content阶段执行,set指令在rewrite阶段执行.如何验证

我们重新编译nginx,使其带上debug功能,./configure --with-debug, 随后设置nginx日志打印等级为debug

在重新访问刚才的url后,就会发现一大堆debug日志,通过
```shell
grep -E 'http (output filter|script (set|value))' error.log
```
即可看到一下信息
```
2018/02/07 18:19:51 [debug] 25367#0: *8 http script value: "32"
2018/02/07 18:19:51 [debug] 25367#0: *8 http script set $a
2018/02/07 18:19:51 [debug] 25367#0: *8 http script value: "64"
2018/02/07 18:19:51 [debug] 25367#0: *8 http script set $a
2018/02/07 18:19:51 [debug] 25367#0: *8 http output filter "/test?"
2018/02/07 18:19:51 [debug] 25367#0: *8 http output filter "/test?"
2018/02/07 18:19:51 [debug] 25367#0: *8 http output filter "/test?"
```
可以看到一条set指令对应一组http script value/set日志,在进行两次set后才执行echo指令对应的output filter日志,由此看出set指令都是在echo之前执行的
那么怎么确定一个指令的执行阶段呢?
* 看文档(phrase关键词)
* 看源码(就是这么调)

### rewrite阶段
这个阶段nginx只要对一些请求进行修改,或者说声明一些后续需要使用的nginx变量,当然具体你要干啥,你很多都能干,毕竟你能嵌一个lua脚本进去.nginx_rewrite模块中的指令如果配置在location中大部分都在rewrite阶段执行,如果配置在server阶段,则运行在server_rewrite阶段,在这一小节,我们要介绍几个在rewrite阶段可以与nginx_rewrite可以混用依次执行的模块指令!
#### set_unescape_uri
```nginx
location /test {
    set $a "hello%20world";
    set_unescape_uri $b $a;
    set $c "$b!";

    echo $c;
}
```
运行结果
```
hello world!
```
查看日志
```shell
grep -E 'http script (value|copy|set)' error.log
```

```shell
2018/02/08 12:02:53 [debug] 19525#0: *13 http script value: "hello%20world"
2018/02/08 12:02:53 [debug] 19525#0: *13 http script set $a
2018/02/08 12:02:53 [debug] 19525#0: *13 http script value (post filter): "hello world"
2018/02/08 12:02:53 [debug] 19525#0: *13 http script set $b
2018/02/08 12:02:53 [debug] 19525#0: *13 http script copy: "!"
2018/02/08 12:02:53 [debug] 19525#0: *13 http script set $c
```
由日志我们可以看到前两行对应set命令,中间两行对应set_unescape_uri,最后两行对应set,所以set_unescape_uri的执行顺序穿插在了两条set中间.

#### set_by_lua
这个指令有什么用呢:可以通过内联一串lua代码给nginx变量赋值.
```nginx
location /test {
	set $a 32;
	set $b 56;
	set_by_lua $c "return ngx.var.a + ngx.var.b";
	set $equation "$a + $b = $c";

	echo $equation;
}
```
运行结果
```
32 + 56 = 88
```
有上面的结果看出,set_by_lua执行在了set命令中间

#### ngx_array_var
#### ngx_encrypted_session
---
以上一些nginx命令可以与ngx_rewrite命令混用,上面提到的这些第三方模块都采用了特殊的技术，将它们自己的配置指令“注入”到了 ngx_rewrite 模块的指令序列中（它们都借助了 Marcus Clyne 编写的第三方模块 ngx_devel_kit),但是其余的大部分的nginx命令都无法通过这种方法,来保证执行的顺序,所以nginx配置中的大部分命令都不应该和与顺序强依赖,下面介绍一些与顺序无关的几个命令

#### more_set_input_headers
这个命令有什么用:这个命令可以在rewrite阶段改写http请求头,并且他总是运行在rewrite阶段末尾
```nginx
location /test {
    set $value dog;
    more_set_input_headers "X-Species: $value";
    set $value cat;
    echo "X-Species: $http_x_species";
}
```
运行结果为:
X-Species: cat
所以这就说明了more_set_input_headers执行在了set $value cat的后面,**再单个请求阶段内部,一般会以nginx请求模块内部划分子阶段**

#### rewrite_by_lua
```nginx
loacation test/{
    set $a 1;
    rewrite_by_lua "ngx.var.a = ngx.var.a + 1";
    set $a 56;
    echo $a;
}
```
运行结果:57
可以看出rewrite_by_lua运行在了set后面,其实他也运行在rewrite阶段的末尾,那么问题来了,如果都运行在rewrite阶段的末尾,那么是谁先运行呢,答:不知道
### access阶段
access阶段一般都是进行一些访问控制的命令,比如检查访问ip是否合法等等
#### deny/allow
这两个命令用来控制

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
## nginx执行阶段
### rewrite阶段
### access阶段
### content阶段

# 拓展开发
## openresty



