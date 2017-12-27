---
title: 每天一个Linux命令
date: 2017-12-27 08:30:10
categories: 
- Linux
tags:
---

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

#### -ctime
change time文件状态改变时间，指文件的i结点被修改的时间，如通过chmod修改文件属性，ctime就会被修改。
![-name](/uploads/每天一个Linux命令/find-ctime.png)
可以看到貌似对文件进行编辑也会造成change time的变化

#### -atime
access time查看文件的状态
