---
title: 每天一个Linux命令
date: 2017-12-27 08:30:10
categories: 
- Linux
---
## perf
## top
常用的查看系统状态的命令,楼下的**htop**貌似更吊一点,更加人性化
![-name](/uploads/每天一个Linux命令/top.png)
### 解释/送上到下
top - xx:xx:xx 当前系统时间
up xxdays xx:xx 系统已运行时间
n user  n个用户当前登录
load average
> load average这个参数还是充满了文章,直译过来就是机器cpu平均负载
> 该参数会带三个值 x,y,z 分别表示在1min,5min,15min内的机器平均负载
> 那么机器平均负载是什么呢:特定时间内,运行队列中的平均进程数,哪些进程会被算在里面
> 当前正在运行的进程,当前正在排队的进程,比如当前运行进程12,排队进程13,那么load就是25,load average不知道怎么算
> load average的值为0-**1**,1的时候表示所有进程都在运行中无等待
> 多核cpu,的load average值会与核数成正比
> 总的来说,我们要搞清楚load average描述的是什么事情,**一段时间内,cpu所平均处理的进程数**
> 那他和cpu的压力什么关系,两个维度,可能一个进程占用了cpu,但是挂机不干活,那么cpu的压力就打打减小了

tasks 当前进程统计信息
* total 总数
* running 正在运行
* sleeping 睡眠进程数
* stop 停止进程数
* zombie 僵尸进程数

cpu cpu时间花费统计信息% 百分比信息
* us 用户空间时间占用cpu百分比
* sy 内核空间时间占用cpu百分比
* ni 用户进程空间内改变过优先级的进程占用CPU百分比
* id 空闲cpu占比
* wa 等待输入输出的cpu时间百分比
* hi 硬中断
* si 软中断
* st 实时

mem 内存使用状况统计
* total内存总量
* used 使用的物理内存总量
* free 空间内存总量
* buffers  用作内核缓存的内存量

swap 交换区信息
* total 交换区总量
* used 已使用交换分区总量
* free 空闲的交换分区总量
* cache 缓冲的交换分区总量

重点扯一下 buffers和cached
A buffer is something that has yet to be "written" to disk. A cache is something that has been "read" from the disk and stored for later use.

### 进程描述部分
PR 进程优先权

## htop
## ps
## ${} && $()
### $()
执行括号中的代码返回的结果
#### 与``的异同
https://blog.csdn.net/foreordination_/article/details/73740966
### ${}
访问括号中的变量返回的结果
## readlink
主要用来找出符号链接所指向的位置,可以获取到绝对路径
### -f
-f 选项可以递归跟随给出文件名的所有符号链接以标准化，除最后一个外所有组件必须存在
## patchelf
修改动态链接库的地址
patchelf --set-interpreter gcc_lib_dir/ld-linux-x86-64.so.2 $ngx_bin_path
## chrpath
## pushd && popd
两个很简单的命令,一般用于多个文件路径需要频繁切换的状况
### pushd
基本功能很简单:每次pushd xxxx,xxxx文件将会进入栈顶;每次pushd 栈顶两元素互换位置,再次进入栈顶元素路径
### popd
这个就更简单了,每次popd,栈顶元素出栈,进入当前栈顶的路劲
![-name](/uploads/每天一个Linux命令/pushd&&popd.png)

## git --diff && git --patch
忽然发现hin尴尬,貌似公司nginx项目使用git做的diff和patch
## diff&&patch
看公司nginx项目发现通过diff patch的方式对源码做了修改,很有用的命令,学一下




## route
mirror机路由不到某台机器,大帅哥尝试用这个命令拯救一下,虽然最后失败了
## ifconfig
天天在用的命令,但是没怎么仔细看懂过,貌似大多都这样
![-name](/uploads/每天一个Linux命令/ifconfig.png)
###  参数解析
Link encap: 暂时理解成网络类型,常见的有 Ethernet,Local Loopback
HWaddr:网卡地址(mac地址)
inet addr:网络地址
inet6 addr:ipv6地址,简单明了
UP（代表网卡开启状态）RUNNING（代表网卡的网线被接上）MULTICAST（支持组播）MTU:1500（最大传输单元）：1500字节
RX: 接受数据包统计 下面还有接收字节数
TX: 发送数据包统计 下面还有发送字节数
Bcast:广播地址
Mask:掩码

### lookback
骚三的电脑ifconfig出来有两个lo,很好,你成功得引起了我的注意,事情要从lookback是什么说起
简单来说,顾名思义,环回地址,就是回到原来地方的地址,我们在正常的互联网操作中,都会使用ip地址表示发送方,但是!如果你的设备处于断网状态,专业点说网络管理设备down了,那你怎么是用网络服务呢,最简单的例子,你不联网想开发个网站,不都是127.0.0.1搞起吗.
不走网关,直接通过cpu连上本地的ip,就是环回地址。
注意,环回地址是需要系统配置的.
所以有很多路由器有一些奇怪的现象,ping自己居然耗时比ping别人还长,因为人家没有配环回,出去绕了一圈回来的
## tcpdump
总的来说,一个抓包工具
### -i xxx
指定监听的网卡.
### host 
指定监听的网站
以豆瓣为例
![-name](/uploads/每天一个Linux命令/tcpdump01.png)
### port
监视指定主机与端口的数据包
**注意:**不知道为什么 tcp后面必须先跟port然后再写ip才能进行监听
![-name](/uploads/每天一个Linux命令/tcpdump04.png)

### src
指定发送方host
指定从豆瓣发过来的
![-name](/uploads/每天一个Linux命令/tcpdump02.png)


### host
**指定监听的域名,也可以是ip**

### -X



## ldd
nginx编译之后上传mirror启动失败,大帅哥秀了一手ldd定位编译文件问题,必须记一下
### 定义
ldd=(list, dynamic, dependencies)
意思就是列出动态库依赖关系
![-name](/uploads/每天一个Linux命令/ldd.png)
然后你就发现了这个二进制文件所依赖的所有动态库,以及他们所在的路径,但是你会发现一个很牛逼的动态库
**linux-vdso.so.1**他居然没有路径,好奇之下查了下资料,其实是把内核态的系统调用映射到用户态的系统空间中。

## nslookup
>师傅秀了一手nslookup,还不告诉我。查之
作用:查询域名
## zcat
今天(20180109)师傅让我查一手线上问题,一番折腾,随后想看师傅秀一手shell,结果师傅跪了,zcat,zgrep都没出来,故在此总结

zcat会把所有的.gz文件的内容都输出到标准输出上
zgrep可以grep单个.gz文件,师傅跪了是因为zgrep了.tar.gz文件包
## zgrep
## type 
## which
## locate

## whereis
定位一个命令的二进制文件路径,文档路径,源码路径
### option
#### -b
定位可执行文件
![-name](/uploads/每天一个Linux命令/whereis-b.png)
#### -s
定位源代码文件
#### -m
定位文档文件
![-name](/uploads/每天一个Linux命令/whereis-m.png)

## xargs

## find
作用:在指定目录下查找**文件/文件夹**.
```shell
find [filepath] [option]
```
### option
#### -name
紧跟模式匹配,输出文件名符合该模式的文件
![-name](/uploads/每天一个Linux命令/find-name.png)
#### -perm
按照文件的读写权限属性来进行筛选
![-name](/uploads/每天一个Linux命令/find-perm.png)
#### -user -nouser
按照文件属主查找文件
![-name](/uploads/每天一个Linux命令/find-user.png)
为了查找属主帐户已经被删除的文件，可以使用-nouser选项。不试了

#### -group -nogroup
和user差不多,不试了
#### -mtime
>File's data was last modified n*24 hours ago.  See the comments for  -atime  to  understand how rounding affects the interpretation of file modification times.

表示这个文件的修改时间 后面跟时间<font color=red>+n,-n</font> 
+表示在n小时前修改的
-表示在n小时内修改的
![-name](/uploads/每天一个Linux命令/find-mtime.png)
-mmin类似

#### -ctime 
change time文件状态改变时间，指文件的i结点被修改的时间，如通过chmod修改文件属性，ctime就会被修改。
![-name](/uploads/每天一个Linux命令/find-ctime.png)
可以看到貌似对文件进行编辑也会造成change time的变化

-cmin类似 换成分钟

#### -atime
access time查看文件的状态 
![-name](/uploads/每天一个Linux命令/find-atime.png)

-amin类似

#### -type
根据文件类型来查找
-type f 文件类型
-type d 文件夹类型
b/d/c/p/l/f  
![-name](/uploads/每天一个Linux命令/find-type.png)

#### -empty
 查找空文件,文件夹
![-name](/uploads/每天一个Linux命令/find-empty.png)

#### -size
按照文件大小搜索,需要写明单位
+-好和时间一样,+大-小
![-name](/uploads/每天一个Linux命令/find-size.png)

#### -exec
>-exec command ;
Execute command; true if 0 status is returned.  All following arguments to find are taken to be arguments to the command until an argument consisting of `;' is encountered.  The string `{}' is replaced by the current file name being processed everywhere it occurs in the arguments to the command, not just in arguments where it is alone, as in some  versions  of  find.Both  of these constructions might need to be escaped (with a `\') or quoted to protect them from expansion by the shell.  See the EXAMPLES section for examples of the use of the -exec option.  The specified command is run once for each matched file.  The command is executed in the starting directory.   There are unavoidable security problems surrounding use  of  the -exec action; you should use the -execdir option instead.

作用执行命令:如果返回0状态则为true.
所有被find指令查找到的文件都会被作为**command**命令所处理,command命令需要;作为标示表示结束,**{}**在command表示当前正在被处理的文件。在有些shell版本中需要在命令最后加上\(excaped)或者说quoted字符。详情见例子。

总的来说就是

find xxxxxx -exec command \;(我的版本)
在command中你所要所做的命令操作的对象可以通过{}来替代

![-name](/uploads/每天一个Linux命令/find-exec.png)

#### -newer -cnewer -anewer
-newer -cnewer -anewer <file>
比file m时间, c时间, a时间更靠近当前时间的文本,就是更新的
![-name](/uploads/每天一个Linux命令/find-newer.png)


#### !
最后一个,find可以支持反向查找,在以上匹配命令前,加上!，表示不符合这个条件的查找条件
![-name](/uploads/每天一个Linux命令/find-!.png)

