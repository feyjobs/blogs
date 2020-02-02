---
title: Question
date: 2018-05-31 16:36
categories: 
tags: 
---

# LC_ALL=C
## 具体描述
linux许多脚本中会在开头设置LC_ALL=C,这个命令什么作用

## 解答

# shell for
## 具体描述
for语句后面没有跟遍历的内容例如
```shell
for option
do
    echo $option
done
```
## 解答
shell for不跟遍历内容默认遍历传入参数

# shell case
## 解答
* case匹配了第一个能匹配的选项后不会继续匹配后续的
* case匹配支持正则表达式

# cat << xxx
## 具体描述
没啥好描述的,看不懂啥意思
## 解答
这个问题的关键在于,shell语法中的各种重定向不理解,现在梳理下,
* >简单得说就是重新写入,*覆盖*
* >> *追加*
* < 重新向输入
* << 不仅重定向出入 而且设置**标志位**,表示到标志位之后就停止接收

```shell
cat << aa
> hihkihul
> askdhasud
> alsdhasd
> aa
直到这里才会输出
```

