---
title: ssh登陆
date: 2018-01-08 01:00:00
categories: 
- Linux
---
用上了公司发的新mac,很开森。回家开始折腾和家里的电脑联通，经过小小的折腾终于搞定
## 登陆
首先就是登陆了,mac,linux互相在同一个局域网内,
Linux测确保开启sshd server
mac这边直接在**系统偏好设置-共享-远程登陆**中设置开启共享即可，随后即可双方通过 ssh user@(hostname|ip)进行账号密码登陆了

## 免密码
老是用密码肯定是烦得不行，所以琢磨了下如何设置rsa登陆，下面将描述A免密登陆B机器描述如下
在B机器(可以视为server)中当前账号的 .ssh文件夹下新建一个 **authorized_keys** 文本文件。
将A机器上生成的公钥(一般为id_rsa.pub) 中的内容添加到authorized_keys中(cat id_rsa.pub >> authorized_keys)
将B机器上的authorized_keys文件权限设置为**600**,据其他博客说不是600不可行,还未尝试，至少本宝宝目前添加了这步操作。

至此 A机器上就可以直接 ssh免密码登陆B机器了 



