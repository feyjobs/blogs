---
title:  长连接 短连接 连接服用
date: 2019-05-22 14:51
categories: 
tags: 
---

[TOC]

参与互联网项目的小伙伴经常会碰到几个相似的词语,导致概念混淆,今天花了一下午时间认真琢磨了一下各个的区别.
* tcp keepalive
* http keeplive(与此同时经常出现的还有长短连接的考虑)
* keepalived
<p>
    keepalived是Linux下一个轻量级别的高可用解决方案。顾在此不表重点讲一下tcp和http的keepalive
</p>

## tcp
首先抛出一个自己以前的错误认知:
> tcp长连接

**tcp没有什么长短连接**,只要不出现网络异常,主动断开,tcp连接可以一直跑下去跑到天荒地老

那么tcp的那一堆keepalive参数是干什么的呢?首先看下有那些keepalive参数
```shell
cd /proc/sys/net/ipv4
ll
```
可以看到有三个keepalive参数
* tcp_keepalive_time
* tcp_keepalive_probes
* tcp_keepalive_intvl

<font color=red>tcp_keepalive_time</font>:一个tcp连接空闲了tcp_keepalive_time秒之后,就要怀疑对方的健康状况了,注意:必须在socket上设置了**SO_KEEPALIVE**选项,才会触发这个机制

<font color=red> tcp_keepalive_probes</font>:怎么确认对方的健康状况呢,发送侦测包(probe表示探针,也是星际二中萌萌的神族农民),如果对方能正确返回,那就说明对面还活着,这个参数描述发送几个侦测包(毕竟不要一棍子打死)

![probe](/uploads/nginx/probe.jpg)

<font color=red> tcp_keepalive_intvl</font>:这个描述每隔多少tcp_keepalive_intvl发送一个侦测包

ok,看到这三个参数的意义,我们就明白了,tcp的keepalive是做tcp**健康检查**的

### 测试
#### 正常状况
首先我们使用nc命令启动一个tcp server
```shell
nc -l 2333
```
监听2333端口
然后我们拿python写一段tcp连接代码
```python
import socket
import time
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) 
s.setsockopt(socket.SOL_TCP, socket.TCP_KEEPIDLE, 20)
s.setsockopt(socket.SOL_TCP, socket.TCP_KEEPINTVL, 1)

s.connect(('127.0.0.1', 2333))

time.sleep(61)
```
代码中分别设置了
* SO_KEEPALIVE 启动tcp健康检查
* TCP_KEEPIDLE 空连接20s进行一次检查
* TCP_KEEPINTVL 每隔1s发送一个检查探针
* 还有一个参数tcp_keepalive_probes取系统默认的
这里是
![probe](/uploads/nginx/tcp_keepalive_probes.jpg)
也就是说,空闲20s后开始发送探测报文,每隔1s发一个最多发2个

用tcpdump抓包
```shell
tcpdump -i lo tcp port 2333
```
启动python
首先是三次tcp握手
![handshake](/uploads/nginx/handshake.jpg)

随后等待20s,第一次探测报文发送,server很快回复ok 表示我还活着
![handshake](/uploads/nginx/first.jpg)

然后他们又很无聊得等了20s,又发了一次探测报文,server再次回复ok
![handshake](/uploads/nginx/second.jpg)

最后他们又很无聊得等了20s,又发了一次探测报文,server再次回复ok
![handshake](/uploads/nginx/third.jpg)

程序退出,挥手再见
![handshake](/uploads/nginx/byebye.jpg)

好的,在这个情况下,client与server都愉快得交换了双发意见,达成了互相都健康得共识,接下来看一个不健康的

#### 异常状况
依然用nc命令启动server,python代码也不变,但是我们需要断网使用iptables命令
具体步骤
* 启动tcpddump
```shell
tcpdump -i lo tcp port 2333
```
* nc启动server
* 运行python 完成握手
* **重要** tcp握手之后快速完成端口屏蔽
```shell
iptables -I INPUT -p tcp --dport 2333 -s 127.0.0.1 -j DROP
```
屏蔽127.0.0.1:2333的网络连接,然后就等着下面这一幕
![handshake](/uploads/nginx/failed.jpg)
- 兄弟 你还好吗
- +1s 兄弟 你还好吗
- 死了,发送**rst**报文,表示异常终止

