---
title: 常用Linux命令
date: 2017-07-25 11:14:10
categories: 
- Linux
tags:
---
## sed
### 命令格式
```
sed [options] 'command' file(s)
sed [options] -f scriptfile file(s)
```

#### options
```
 -e <script>  以选项中的指定的script来处理输入的文本文件
 -f <script>  以选项中的指定的script来处理输入的文本文件
 -h          help
 -n          仅显示script处理后的结果
 -v          版本
 -i          会直接编辑文件
```

### command
**sed命令里所有的扩号不做匹配做功能时都是要加上\\的**

## awk
### 命令格式
```
<<<<<<< HEAD
=======
```
>>>>>>> f6328ff52b8d5e997bd0a91aeb412feaad8f949c
### case
```sed
sed -i 's/db_b_rd_select/gaohao/g;s/SHYRQpHavZSB/haohao/g;s/place_waimai/gaohao_place_b_waimai/g;s/10\.19\.145\.222/10.19.161.142/g;s/7220/5100/g' place_waimai.conf
```
awk [options] 'Pattern{action}' file
```
### 选项 options
#### -F:
> 我特么只能说-F简直是个rbq,怎么写都可以

比方说以]为分隔符可以写成

```
awk -F]
awk -F ']'
awk -F "]"
```

指定多个分隔符;,|

```
awk -F'[:,|]'
```

#### -v
用于设置变量的值

```
awk -v FS='#' XXXXXXXX
```
则将awk分隔符设置#
设置**OFS**可以设置输出变量符
![OFS](/uploads/OFS.png)

### awk程序
#### 全局变量
| 变量字段      | 意义          | 
| ------------- |:-------------:| 
| $0      | 当前awk读入的整行文本 | 
| $1      | 第一个参数      | 
| $2 | 第二个参数| 
| NF | number of fields,$0上字段总数| 
| NR | number of Records,awk已读入的数据行数|
| FNR | file number of Records,各文件的行号|
| FILENAME | awk正在处理的文件名| 
| FS | 输入字段分隔符,default \t|
| OFS | 输出字段分隔符,default \t| 
| RS | 输入记录分隔符,指定输入时的换行符|
| ORS | 输出记录分隔符,指定输入时的换行符| 
| ARGC | 命令行参数个数|
| ARGV | 命令行各个参数| 
注意:$是否要转义,在字符串下


#### 格式
```
Pattern1 {Action1}
Pattern2 {Action2}
Pattern3 {Action3}
```
执行pattern,若pattern为true,则执行action
空pattern默认为true
''包含awk程序 不需要转义

##### 关系表达式 Pattern
	- 关系运算符
	- 模式匹配表达式:~ 正则表达式匹配符  !~ 正则表达式**非**匹配符
	- 模式,模式:指定一个行的范围
	- /正则表达式/:使用通配符的拓展集
	- BEGIN:让用户指定在第一条输入记录被处理之前所发生的动作,设置全局变量
	- END:让用户在最后一条数据记录被读取后发生的动作

###### Begin/END
Begin:表示该部分动作在指定文本前执行

```
awk 'BEGIN{print "aaa"}'
```
![BEGIN](/uploads/AWK-BEGIN.png)

END:该部分动作在指定文本后执行

```
awk 'END{print "aaa"}' php-error.log
```
![BEGIN](/uploads/AWK-END.png)
##### 动作 Action
> 动作大多与C语言类似





## sort
```shell
sort [-bucfMnrtk][源文件][-o 输出文件] 
```
| 变量字段      | 意义          | 
| ------------- |:-------------:| 
| -b| 忽略每行开始出现的空格字符 | 
| -u| 去重 | 
| -c| 检查文件是否已经按照顺序排序| 
| -f| 忽略字母大小写| 
| -M| 将前三个字母按照月份进行排序|
| -n| 按照数值大小进行排序|
| -o<file>| 排序后将结果存入文件| 
| -t| 指定排序时所用的栏位分隔字符|
| -k| 选取哪个区间进行排序| 

<<<<<<< HEAD

=======
## find 
```shell
find . -type f -exec dos2unix {} \;
```
>>>>>>> f6328ff52b8d5e997bd0a91aeb412feaad8f949c
