[TOC]
WMQ代码解读

# 背景
WMQ由三部分构成：Proxy、Kafka、Pusher。

Proxy负责生产消息，将消息存储到Kafka中。

Kafka负责持久化消息。

Pusher负责从Kafka取消息，将消息发送消费者。

涉及两个开源库：
- sarama：kafka SDK
- kazoo：zookeeper SD
- K

# Proxy
开HTTP服务，回调messageHandler。

流程：
1. 读取Request
2. 构造sarama消息msg
3. 过滤器（白名单）
4. 同步发送到Kafka
5. 或者异步发送到Kafka

# Pusher
Pusher中最小的配置单元为consumerGroup。

每个consumerGroup创建了一个haulerManager。

haulerManager执行generateHaulers，获取topic全部的partitions。

每个partition创建一个Hauler。

在Hauler中初始化sarama，并初始化OffsetManager，由OffsetManager监听offsetCh进行offset提交。

根据Type（PUSH or PULL）进入Loop。

- 在PUSH模式中
1. 根据windows配置初始化相当数量的bearers。
2. 通过sarama接口sarama.PartitionConsumer.Messages获取一条kafka消息。
2. 将消息发送到bearer的channel中。
3. 之前初始化bearer时，已经开了协程接收msg，收到之后将msg打包HTTP发送给消费者。
4. 提交offset。

- 在PULL模式中
1. 在main函数中开HTTP服务，拉取消息回调messageHandler。
2. 在messageHandler中，将msgReq发送给PullCh拉取消息。
3. Hauler获取PullCh拉取消息后，发送到request.Messages。
4. messageHandler取得request.Messages返回消费者。


group.topic => hauler manager => loop generateHaulers => newHauler => loop

Channel：
    probe
    skipCh
    markCh
    statusCh
    offsetCh
    event
    rbsignal

Coroutine：
    go m.loop()
    go hauler.loop()
    go bearer.start()
    go om.loop()

# 特性
## SKIP命令
## MARK命令
## STATUS命令
## 重新入队
## 延时消息

# FAQ
- 两次req.Body.Close()，其实应该0次
- PROXY的ZK、异常处理。
- Huskar注册时机
- 平滑重启
- 文档配置不全啊大哥
-   ```
        //compute hash key
        index := int32(h.hash.Sum32()) % int32(h.conf.Window)
        if index < 0 {
            index = -index
        }
    ```
- partConsumer怎么用
- PULL模式缓存消息
- TODO
- Rebalance
- prob怎么触发
- proxy的newDispatcher怎么回事
- 配置热加载
- 水平扩展

