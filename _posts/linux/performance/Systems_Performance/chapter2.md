---
title: 方法
date: 2019-11-22 14:17
categories: 
tags: 
---

## 方法
### USE
核心思想, 在问题排查过程中, 以资源为轴, 列举分析各种资源类型, 以及各种资源的关键指标(use), 来分析问题
* 资源
* 使用率(utilization): 60%以上就要预警了, 因为一般提供的是平均指标, 60%的使用率,很可能意味着某些短时间达到了100%
* 饱和度(saturation): 任何饱和现象都是问题
* 错误(error): 任何错误都是问题

### 工作负载特征归纳
辨别负载的来源, 有时候系统出了问题不是架构有问题, 而是负载本身就超过了系统的范围

### 向下挖掘分析
* 检测 高层级的统计数据
* 识别 根据问题, 缩小研究范围
* 分析 
5 why

### 延时分析
将延时分段, 然后深入分析关键延时时段, 需要与向下挖掘类似, 自顶向下深度分析

### R方法
oracle 性能分析方法

### 事件追踪
系统的操作本质是一些离散的事件, cpu指令, 磁盘IO, 网络包的收发等等. 又是系统的指标,汇总数据是看不出问题的所在的, 需要我们
深入系统事件,进行问题排查.举个栗子:
#### 网络事件
tcpdump逐包排查

#### 存储设备
iosnoop排查系统问题

#### 系统调用
strace

在进行系统事件追踪时, 故障时间段的上下时间段的事件也非常值得一看, 因为有些情况下, 是由于先发事件的问题,
导致了问题事件的表象, 实际上还是先发问题的根因造成了这个现象

### 基础线性统计
将各个性能指标与时间等变量作函数曲线,观察变化

### 静态性能调整
就是检查各种系统预加载或者初始系统选项

### 缓存调优

### 微基准测试

## 建模
分析系统性能三巨头:
* 生产系统的观测
* 试验性系统的观测, 压测, 仿真
* 根据上面的汇总的数据,进行数学建模, 进行数学预测

### 可视化
根据性能随着规模扩大的变化,查看斜率的变化,基础的数学知识

* y=ax+b 线性扩展, 完美的扩展结果
* y=lnx 竞争关系,某些公用的组件回造成扩容时的各个节点的竞争效应, 减小扩容效果
* y=(x-1)^2 分布式关系, 随着节点的增加, 节点之间数据同步备份的消耗超过了扩容带来的增益
* 拐点, 某个因素碰到了资源的制约点, 从而改变扩展曲线
* 扩容上线

### amdahl扩展定律
### 通用扩展定律
### 排队理论
上面上个主要描述了, 各种基于现有数据的拟合模型, 然后进行系统分析, 太特么学院派了, 遭不住

## 容量规划
### 资源极限
遍历资源列表, 根据当前资源的使用率, 判断预估这个资源所能支持的最大流量， 然后进行扩容

资源列表:
#### 硬件
* cpu使用率
* 内存使用率
* 磁盘iops
* 磁盘吞吐量
* 图盘使用率
* 网络吞吐量
#### 软件
* 虚拟内存使用情况
* 进程/线程/任务
* 文件描述符

### 因素分析
怎么判断为了支持一定程度的负载需要什么机器配置呢,内存, cpu, 磁盘, 网卡, 操作系统版本, 文件系统设置等等很多选项, 逐一尝试
工作量太大了.
方法:
* 将所有配置都设置成最高
* 逐一改变配置, 测量对性能的影响
* 根据测量的结果, 对每个因素对性能的影响以及节省的成本作分析
* 从最高性能为起点, 选择最低成本因素, 确保最后的性能可以支持负载
* 测试配置
## 统计
各种数学学院派的东西, 先跳过

## 监视
### 基于时间的统计
### 监视产品
* 自研发, 运行在服务器上, 然后上传, 但是有损系统性能
* snmp协议,  减少客户端程序运行
### 自启动信息
还有一些信息是操作系统启动以来自带的信息

## 可视化
各种图例信息

