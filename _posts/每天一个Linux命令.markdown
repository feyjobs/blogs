---
title: 每天一个Linux命令
date: 2017-12-27 08:30:10
categories: 
- Linux
---
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

