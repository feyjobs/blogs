---
title: docker network
date: 2021-04-13 22:13
categories: 
tags: 
---
docker网络通信方式是理解kubernetes网络原理的基础, 所以本系列文章主要分成两块
* docker本地网络通信原理
* 跨node网络通信原理


## docker本机容器通信
### 基本原理介绍
在介绍docker本地网络通信之前, 首先需要介绍一个虚拟设备veth-pair
顾名思义, veth-pair就是一个成对出现的虚拟设备， 他们一端连接着网络协议栈, 另一端连接这彼此, 大概就是这个意思
![dockerps](/uploads/linux/k8s/docker/veth.png)

#### 直连
测试一下
首先我们创建两个namespace
```shell
ip netns add ns1
ip netns add ns2
```

创建一个veth-pair
```shell
ip link add veth0 type veth peer name veth1
```
通过以下命令可以查看新创建的veth-pair设备
```shell
ip a s
```
![dockerps](/uploads/linux/k8s/docker/vethpaircreate.jpg)

然后我们将两个设备移动到相应的namespace
```shell
ip link set veth0 netns ns1
ip link set veth1 netns ns2
```

接下来给两个网络设备添加ip地址
```shell
ip netns exec ns1 ip addr add local 192.168.10.200/24 dev veth0
ip netns exec ns2 ip addr add local 192.168.10.201/24 dev veth1
```
![dockerps](/uploads/linux/k8s/docker/vethipcreate.jpg)

然后我们启用这两个虚拟网络设备
```shell
ip netns exec ns1 ifconfig veth0 up
ip netns exec ns2 ifconfig veth1 up
```

然后我们在ns1中ping对端ip
```shell
ip netns exec ns1 ping 192.168.10.201
```
![dockerps](/uploads/linux/k8s/docker/vethping.jpg)
通过 无问题
好了现在我们大概理解了vethpair的作用, *可以跨过namespace进行网络通信*

#### 网桥
ok 现在我们通过veth直接连接方式完成了namespace之间的网络通信, 但是就像现实中网络不可能两个需要联网的人自己拉一根网线一样, 在虚拟设备的网络规划中
也需要一个交换机的角色, 来帮助管理大量互联的网络设备, 由此诞生了*网桥*设备, 网桥可以理解成一个虚拟的网络交换机
![dockerps](/uploads/linux/k8s/docker/linuxbridge.png)

我们测试下通过网桥打通不同namespace的网络通信
```shell
ip l a br0 type bridge
ip l s br0 up 
```
创建veth-pair设备
```shell
ip l a br-veth0 type veth peer name brvethotherend0
ip l a br-veth1 type veth peer name brvethotherend1
```

将veth设备放到相应的namespace中
```shell
ip l s br-veth0 netns ns1
```
将veth设备另一段放到网桥上
```shell
ip l s brvethotherend0 master br0
ip l s brvethotherend0 up
```
对br-veth1执行相同的操作

为两个namespace下的设备绑定ip, 并且启用
```
ip netns exec ns1 ip a a 192.168.11.200/24 dev br-veth0
ip netns exec ns1 ip l s br-veth0 up
```
另一侧进行同样的操作

最后我们在ns1中ping 192.168.11.201看能否成功
```shell
ip netns exec ns1 ping 192.168.11.201
```

### 基本环境介绍
启动两个docker容器
![dockerps](/uploads/linux/k8s/docker/dockerps.jpg)

宿主机查看设备情况
![dockerps](/uploads/linux/k8s/docker/items.jpg)
可以看到docker创建了一个id为docker0的网桥, 这个网桥设备会起到本节点docker容器间网络通信纽带的作用

查看网桥详细信息
![brctl](/uploads/linux/k8s/docker/brctl.jpg)
可以看到docker0网桥挂载了两个vethpair分别是, veth792b0d2 vethcc3a695

现在有了一个问题, 我们创建了很多个vethpaire, 我们怎么知道哪个vethpair和哪个docker中的eth0设备关联在一起呢
![iplink](/uploads/linux/k8s/docker/iplink.jpg)
可以看到在宿主机上的vethpair有一个link id
![iplink](/uploads/linux/k8s/docker/iplinkdocker.jpg)
而在docker中的eth0设备也存在一个linkid

查看宿主机的路由表
![route](/uploads/linux/k8s/docker/route.jpg)
通过路由表我们可以看到172.17.0.0/16网段的请求都会被打向docker0网桥, 然后docker0设备会帮我们路由到相应的docker容器

### docker通信测试
现在我们创建了两个容器, 然后让他们进行本地容器间通信
我们在容器nginx-2中运行
```
curl 172.17.0.2
```
然后在容器内监听eth0网卡的网络传输
```
tcpdump -i eth0 -w cap
```
![route](/uploads/linux/k8s/docker/dockertcpdump.jpg)
现在我们可以看到在容器内发送一个http请求被转发到另一个容器的过程, 同时还意外捕获了arp数据包刷新ip对应mac地址的数据包


之后我们在宿主机监听docker0网桥,同样可以捕获到相同的数据流
![route](/uploads/linux/k8s/docker/cap-curl.jpg)

由此我们验证了docker容器在单机环境下的网络通信数据流。
![route](/uploads/linux/k8s/docker/dockerlocal.png)



