---
title: network
date: 2020-07-21 14:46
categories: 
tags: 
---

kubernetes系统中的网络多且复杂,涉及各种虚拟化设备, 需要专门研究一下, 彻底搞清楚.

## 相关概念
### vxlan
要梳理vxlan是什么东西, 我们先来看局域网络的发展史, 忽然发现这个虚拟化方面的东西, 和容器化关系不大, 下次再说吧

#### lan && vlan
一开始我们有一个最原始的的局域网架构
![lan](/uploads/k8s/network/lan.png)
多个终端连接到一个集线器, 由于集线器是一个工作在1层(物理层)的网络设备, 他的作用是对收到的某一个端口的信号进行维持-放大-发送到每一个链接的端口上

但是这样所有终端设备都连在一个集线器上, 而每次只能有一个设备进行信号发送, 这就造成信号冲突, 所以那帮网络大佬设计了一大堆冲突避免算法, 比方说csma/ca, csma/cd
为了减少冲突, 就引入了交换机, 每个交换机可以连接多个网络终端, 每个与交换机相连的终端都成为该子网的冲突域
![lan](/uploads/k8s/network/switch.png)


![lan](/uploads/k8s/network/switch2.png)
上图可以看到, 每个交换机的端口都形成了一个冲突域, 第一个冲突域中多个终端依然走同一个集线器, 造成信号冲突. 

引入交换机之后, 虽然冲突域划分了, 也就减少了冲突域的大小, 但是他们依然在同一个广播域下面, 即每个终端设备发送广播, 改交换机下所有终端都能收到

分割广播域的任务就交到了路由器的身上, 广播发送时都会使用255.255.255.255作为目的地址，这时路由器不会转发这个数据包到其他端口中。
![lan](/uploads/k8s/network/router.png)

所以一个lan就是一个交换机下一组终端组成的局域网
那么vlan的这个v是作用在哪里的呢?
vlan其实是一个交换机, 虚拟出多个虚拟交换机. 如下所示, 该虚拟交换机的虚拟了3个交换机, 组成了3个广播域
![lan](/uploads/k8s/network/vlan.png)

那么vlan有什么问题呢?
vlan的实现方法在IEEE802.1q中做了详细描述, 其中对vlan标签帧规定如下
|Preamble(7bytes)|SFD(1bytes)|DA(6bytes)|SA(6bytes)|TPID(2bytes)|TCI(2bytes)|Type Length(2bytes)|Data(42~149bytes)|CRC(4bytes)|
其中TCI字段占2bytes, 其中3位用于User Priority, 1位用于CFI, 最后剩下12位作为vlan id, 所以一个物理交换机最多可以虚拟出2^12=4096个vlan.



## 参考链接
[VLAN原理详解](https://blog.csdn.net/phunxm/article/details/9498829)

