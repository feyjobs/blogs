---
title: 存储层次结构
date: 2020-02-05 00:38
categories: 
tags: 
---

## 程序的机器级表示
因为想学下AT&T汇编语言 所以来看看这章

### 磁盘存储
#### 磁盘速度
> 磁盘上读信息的时间为毫秒级,比DRAM慢10万b倍,比SRAM慢100万倍

#### 磁盘结构

![盘片视图](http://upload-images.jianshu.io/upload_images/5902351-8cf0ae080447373a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

  磁盘有盘片组成,盘片表面覆盖有磁性记录材料,盘片中央有一个旋转的主轴。
  磁盘表面由一组称为磁道的同心圆组 ,每个磁道被划分为一组扇区。每个扇区包含相等数量的数据位(512byte)
（要死了/之前一直以为扇区是圆心到弧的完整扇形,原来只是一小条）。扇区之间有一些间隙（gap）分隔开,这些间隙中不存储数据位。间隙存储用来表示扇区的格式化位。

![多个盘片视图](http://upload-images.jianshu.io/upload_images/5902351-7f54a711b0bc2f78.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
    磁盘驱动器:分装在密封包装中的多个盘片。
    柱面:所有盘片表面上到主轴中心的距离相等的磁道的集合。

#### 磁盘容量
    记录密度：磁道每一英寸的断种可以放入的位数
    磁道密度:从盘片中心出发半径上一英寸的段内可以有的磁道数
    面密度:记录密度与磁道密度的沉积
 
初设计:每个磁道分为数目相同的扇区,扇区的数目由最靠内的磁道能记录的扇区数决定。
> 问题：面密度大的时候太尼玛浪费了，到了外层磁道间隙大过天

多区记录技术:将柱面的集合分为互不相交的记录区,每个区包含一组连续的柱面。每个区中的每个柱面中的没调磁道有相同数量的扇区,扇区数有该区中最里面的磁道所能包含的扇区数决定。
> 通过分出不同的记录区,使得外层的记录区能有更多的扇区数 但是= =就不能固定间隙空间大小 每个柱面的扇区数都不一样咩(╯‵□′)╯︵┻━┻ 等等! 注意虽然扇区数越往外越多 但是扇区的大小是不变的

磁盘容量 = 字节数/扇区 \* 平均扇区数/磁道 \* 磁道数/表面 \* 表面书/盘片 \* 盘片数/磁盘
> 稀奇古怪的写法= = 就是磁盘的盘片数\*每个盘片的表面数\*每个表面的磁道数\*每个磁道的平均扇区数\*每个扇区的字节数

> p.s.K M G T这样的前缀含义依赖于上下文。对于DRAM和SRAM容量相关的 为2次幂,对于与网络或磁盘这样的I/O设备容量相关的计量单位,为10次幂

#### 磁盘操作

![盘片视图](http://upload-images.jianshu.io/upload_images/5902351-d39eed62b31a89be.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

> 寻道时间:为读取某目标扇区的内容,传动壁轴线将读/写头定位到目标扇区的磁道上。转动传动壁的时间为寻道时间。T<sup>avg seek</sup>
