---
title: PHP常用函数
date: 2017-07-05 14:43:10
categories: 
- Code
- PHP
tags:
---

## 时间相关函数

### date
>string date(string $format [,int $timestamp])

将$timestamp时间戳(不传的话默认当前时间戳)按照$formate格式,格式化日期

### strtotime
>int strtotime(string $time[, int $now = time()])

将任何字符串的日期时间描述解析为unix时间戳,$time时间描述字符串,$now当前时间戳
**注意$time描述的字符串支持Y-m-d格式,不支持Ym,md等格式,但支持Ymd格式**
Ymd格式转化成时间戳只能,strtotime("Ym01 +1 month")等方式,通过补齐成Ymd格式来进行月份运算.

## 数组相关函数
### array_push
> int array_push(&$array, mixed $value1[,mixed $value2...])

单条语句的效果等同于
```
	$array[] = $value1;
```
**如果只调用单条语句还不如用上面的代码方法,不会有函数调用的消耗**
**如果$array不是一个数组,会push失败,而上面的代码可以创建数组**

### array_intersect_key
> array array_intersect_key ( array $array1 , array $array2 [, array $... ] )

该方法返回所有出现在$array1中,且key为所有输入数组的交集


