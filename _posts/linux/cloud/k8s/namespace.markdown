---
title: namespace
date: 2020-11-03 14:49
categories: 
tags: 
---
玩过docker都知道容器的两大基石, 一个是Namespace一个是cgroup,这两个都有什么作用呢, 怎么运作的呢, 这个博客将重点讲下namespace的作用
本文内容基于一个YouTube上的namespace[介绍视屏完成](https://www.youtube.com/watch?v=-YnMr1lj4Z8)

研究一个东西，无非从两个方向入手, 文档&&源码

那就先看文档, 首先看看docker自己介绍自己的大概
> Docker is written in the Go programming language and takes advantage of several features of the Linux kernel to deliver its functionality.

* go写的
* 基于一些linux内核 feature做的

紧接着就是介绍namespace
> Docker uses a technology called namespaces to provide the isolated workspace called the container. When you run a container, Docker creates a set of namespaces for that container.

docker使用linux namespace技术去创造一个隔离的工作环境, 称之为container. 如果你运行一个容器, docker会为那个容器创造一系列的namespace

> These namespaces provide a layer of isolation. Each aspect of a container runs in a separate namespace and its access is limited to that namespace.

这些命名空间提供了一层隔离效果。 每个容器每一个方面都被限定在某一个隔离的namespace中
那么有哪些namespace类型呢, docker主要使用了以下这些
* The *pid* namespace: Process isolation (PID: Process ID).
* The *net* namespace: Managing network interfaces (NET: Networking).
* The *ipc* namespace: Managing access to IPC resources (IPC: InterProcess Communication).
* The *mnt* namespace: Managing filesystem mount points (MNT: Mount).
* The *uts* namespace: Isolating kernel and version identifiers. (UTS: Unix Timesharing System).

接下来就以pid namespace为例看看namespace怎么运作的, 
首先我们进入容器运行`watch ps ax`,可以看到如下图所示
![容器内watch](/uploads/namespace/container_watch.jpg)
我们可以看到容器内一个12517的watch进程, 但是我们都知道容器其实本身就是一个进程, 那我们就去看看在宿主机里这个进程长什么样运行`ps aux|grep watch`
![宿主机watch](/uploads/namespace/host_watch.jpg)
可以看到宿主机里是一个3166的进程, 并且也可以清楚的看到容器内的京城无法查看宿主机的进程

接下来我们来看下进程的继承树, 通过`pstree -p`命令
```
systemd(1)─┬─AliSecGuard(1437)─┬─{AliSecGuard}(1438)
           │                   ├─{AliSecGuard}(1439)
           │                   ├─{AliSecGuard}(1441)
           │                   ├─{AliSecGuard}(1443)
           │                   ├─{AliSecGuard}(1444)
           │                   └─{AliSecGuard}(8715)
           ├─agetty(631)
           ├─agetty(632)
           ├─aliyun-service(24632)─┬─{aliyun-service}(24634)
           │                       └─{aliyun-service}(24635)
           ├─atd(611)
           ├─auditd(15078)───{auditd}(15079)
           ├─bash(14803)───sleep(4913)
           ├─chronyd(572)
           ├─containerd(28005)─┬─containerd-shim(18789)─┬─bash(18839)
           │                   │                        ├─bash(19001)───watch(3166)
           │                   │                        ├─{containerd-shim}(18790)
           │                   │                        ├─{containerd-shim}(18791)
           │                   │                        ├─{containerd-shim}(18792)
           │                   │                        ├─{containerd-shim}(18797)
           │                   │                        ├─{containerd-shim}(18798)
           │                   │                        ├─{containerd-shim}(18799)
           │                   │                        ├─{containerd-shim}(18800)
           │                   │                        ├─{containerd-shim}(18801)
           │                   │                        └─{containerd-shim}(18894)
           │                   ├─{containerd}(28006)
           │                   ├─{containerd}(28007)
           │                   ├─{containerd}(28008)
           │                   ├─{containerd}(28009)
           │                   ├─{containerd}(28010)
           │                   ├─{containerd}(28011)
           │                   ├─{containerd}(28012)
           │                   ├─{containerd}(28013)
           │                   ├─{containerd}(28014)
           │                   ├─{containerd}(28015)
           │                   ├─{containerd}(28016)
           │                   ├─{containerd}(28017)
           │                   ├─{containerd}(28018)
           │                   ├─{containerd}(28020)
           │                   ├─{containerd}(28234)
           │                   ├─{containerd}(28235)
           │                   ├─{containerd}(28262)
           │                   ├─{containerd}(30566)
           │                   ├─{containerd}(31196)
           │                   ├─{containerd}(31304)
           │                   └─{containerd}(16377)
           ├─crond(14951)
           ├─dbus-daemon(562)
           ├─dhclient(823)
           ├─dockerd(28021)─┬─{dockerd}(28022)
           │                ├─{dockerd}(28023)
           │                ├─{dockerd}(28024)
           │                ├─{dockerd}(28025)
           │                ├─{dockerd}(28026)
           │                ├─{dockerd}(28027)
           │                ├─{dockerd}(28028)
           │                ├─{dockerd}(28029)
           │                ├─{dockerd}(28030)
           │                ├─{dockerd}(28031)
           │                ├─{dockerd}(28032)
           │                ├─{dockerd}(28033)
           │                ├─{dockerd}(28034)
           │                ├─{dockerd}(28035)
           │                ├─{dockerd}(28036)
           │                ├─{dockerd}(28343)
           │                ├─{dockerd}(28370)
           │                └─{dockerd}(29004)
           ├─irqbalance(545)
           ├─openresty(21172)───openresty(14285)
           ├─polkitd(541)─┬─{polkitd}(607)
           │              ├─{polkitd}(608)
           │              ├─{polkitd}(615)
           │              ├─{polkitd}(616)
           │              └─{polkitd}(622)
           ├─rsyslogd(890)─┬─{rsyslogd}(942)
           │               └─{rsyslogd}(1030)
           ├─sshd(1322)───sshd(16840)─┬─zsh(16842)───docker(18980)─┬─{docker}(18981)
                                      │                            ├─{docker}(18982)
                                      │                            ├─{docker}(18983)
                                      │                            ├─{docker}(18984)
                                      │                            ├─{docker}(18985)
                                      │                            ├─{docker}(18986)
                                      │                            ├─{docker}(18987)
                                      │                            ├─{docker}(18988)
                                      │                            ├─{docker}(18989)
                                      │                            ├─{docker}(18990)
                                      │                            ├─{docker}(18991)
                                      │                            ├─{docker}(18992)
                                      │                            ├─{docker}(19071)
                                      │                            ├─{docker}(19072)
                                      │                            └─{docker}(32674)
                                      └─zsh(18943)───pstree(4920)
```
上面是我的机器上的运行结果, 在去除一些非必要信息后我们可以看到整个进程系统以systemd进程为起始, fork出各类进程, 所以基本上你也可以非常清楚得明白linux的基本运作原理了, systemd为起始, 各种fork之后生成各种子进程运行各类服务, 其中最下面我们可以看到用于ssh连接的sshd服务, 一个用户连接之后就会fork出一个ssh连接进程, 然后我们就可以随便操作了

然后我们来看我们在容器中运行的`watch ps ax`在宿主机上在哪里
![宿主机pstreepid](/uploads/namespace/pstree.jpg)
我们可以看到这个进程父子链路`systemd->containerd->containerd-shim->bash->watch`

我们来到梦开始的地方*containerd*, 我们继续来找官网介绍
![宿主机pstreepid](/uploads/namespace/containerd.jpg)
可以看到containerd的官方解释: 一个简单, 健壮且可移植的工业级容器运行时。 这个容器运行时就很抽象, 我就暂时把它理解为一个容器管理系统, 就是他！ 负责拉镜像起容器, 杀容器等等操作
那么他是怎么进行这一些列操作的呢, 在containerd的github readme里简单介绍了
> Runtime requirements for containerd are very minimal. Most interactions with the Linux and Windows container feature sets are handled via runc and/or OS-specific libraries (e.g. hcsshim for Microsoft). The current required version of runc is always listed in RUNC.md.

说白了, containerd是通过一个叫做`runc`来进行核心的容器操作的. ok, 到目前为止, 我们理解了docker运行的大概架构图
![docker](/uploads/namespace/docker.jpg)
docker启动之后会启动一个dockerd进程, dockerd进程会拉起一个containerd进程, containerd进程使用runc来进行container的管理

让我们收束世界线, 回到今天的主题, namespace, 那么runc是如何使用namespace来管理containerd的呢, 我们使用`strace`命令对容器启动时的系统调用进行捕获
捕获到的系统调用非常之多, 很多与namespace无关
我们在strace日志中搜索execve调用, 可以看到第一个出来的是
![docker](/uploads/namespace/strace_containerd.jpg)
结合上面的进程父子关系图, 我们可以知道这个就是实际容器进程的父进程.

继续向下搜索, 我们找到了我们梦寐以求的runc

继续往下搜索, 这次搜索的是unshare系统调用, 
![unshare](/uploads/namespace/strace_unshare.jpg)
到这里我们要介绍下这个unshare函数是干什么的
> unshare() allows a process (or thread) to disassociate parts of its
>       execution context that are currently being shared with other
>       processes (or threads).  Part of the execution context, such as the
>       mount namespace, is shared implicitly when a new process is created
>       using fork(2) or vfork(2), while other parts, such as virtual memory,
>       may be shared by explicit request when creating a process or thread
>       using clone(2).
unshare函数使一个进程可以调整与其他进程共享的部分可执行内容, 这里所谓部分的可以执行内容, 比方说mount命名空间, 默认是父子进程共享的,
而其他部分比方说虚拟内存, 必须指定某些系统参数才能共享

> The main use of unshare() is to allow a process to control its shared
> execution context without creating a new process.
unshare的主要用处是允许一个进程在不创建新进程的情况下就创建出一个命名空间

> The flags argument is a bit mask that specifies which parts of the
> execution context should be unshared.  This argument is specified by
> ORing together zero or more of the following constants:
unshare的第三个参数flag标识的进程是哪部分内容与原来的进程脱钩
我们重点来看下*CLONE_NEWPID*

> CLONE_NEWPID (since Linux 3.8)
>              This flag has the same effect as the clone(2) CLONE_NEWPID
>              flag.  Unshare the PID namespace, so that the calling process
>              has a new PID namespace for its children which is not shared
>              with any previously existing process.  The calling process is
>              not moved into the new namespace.  The first child created by
>              the calling process will have the process ID 1 and will assume
>              the role of init(1) in the new namespace.  CLONE_NEWPID
>              automatically implies CLONE_THREAD as well.  Use of
>              CLONE_NEWPID requires the CAP_SYS_ADMIN capability.  For
>              further information, see pid_namespaces(7).
这个flag和带CLONE_NEWPID标识的clone有相同效果, 调用进程为他的子进程创造一个新的PID namespace. 调用进程本身不会被放入这个新的namespace中, 
在调用unshare后第一个子进程, 他的进程号为1, 并成为新namespace下的init进程

ok现在很明显了, 这个进程18910创造的下一个子进程, 就进入新的namespace了, 继续追clone调用
![clone](/uploads/namespace/clone.jpg)
找到了18910诞生了id为18911的子进程!!让我们来康康他干了什么!! 他给自己改头换面换了个名字, 管自己叫INIT
![clone](/uploads/namespace/rename.jpg)
继续往下看, 18911他生儿子了！！儿子的名字还是*2*, 所以很明显在新的namespace里他的pid就是2, 那他在宿主机上的pid是多少呢 18913
![clone](/uploads/namespace/clone2.jpg)
至此为止我们知道了pidnamespace的大概运行流程, containerd execve出runc父进程, runc创造新的namespace, runc父进程cloneINIT进程, INIT进程在新的namespace里撒野
![total](/uploads/namespace/total.jpg)


其他还有很多namespace类型, 都可以在unshare调用里看具体作用, 就不做赘述了
