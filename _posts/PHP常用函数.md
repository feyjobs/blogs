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

### array_sum

<<<<<<< HEAD
=======
### array_shift
```PHP
mixed array_shift ( array &$array )
```
作用:将array的第一个单元移出并作为结果返回,array长度减一,并将其他单元向前移动一位
```PHP
<?php
$array = [1];
$a = array_shift($array);
var_dump($a,$array);
//ans
//int(1)
//array(0) {
//}
```

>>>>>>> f6328ff52b8d5e997bd0a91aeb412feaad8f949c
### array_chunk
稍微玩过点PHP都会知道这是个很好用的数组切割函数，但是今天碰到了个问题没住到过以前
```PHP
array array_chunk ( array $array , int $size [, bool $preserve_keys = false ] )
```
第三个参数**$preserve_keys**可以指定切割后是否保持原有数组的键值,默认为false，需要保持为TRUE
case:
```PHP
$array = {
	'fuck' => 1,
	'you' => 2,
	'bitch' => 3,
	'lich' => 4
}
$ans1 = array_chunk($array, 2);
// $ans1 = [
//		0 => [
//			0 => 1,
//			1 => 2,
//		],
//		1 => [
//			0 => 3,
//			1 => 4,
//		]
//	]
//;
$ans2 = array_chunk($array, 2 true);
//$ans2 = [
//	0 => [
//		'fuck' => 1,
//		'you' => 2,
//	],
//	1 => [
//		'bitch' => 3,
//		'lich' => 4,
//	],
//];
```

## 多进程相关
日常的日志分析,队列处理,数据量很大时,使用多进程处理
> 多进程相关需要pcntl,posix拓展支持

注意点:
- 多进程只能在cli模式下
- 在循环中创建紫禁城,子进程一定要exit,防止子进程进入循环(踩过坑了= =)
- 子进程中打开连接不能copy,使用的还是主进程,需要使用多例模式

### pcntl_fork
> pcntl_fork()
在父进程中返回子进程pid,在子进程中返回0,出错返回-1

### pcntl_wait
>pcntl_wait(int &$status[,int $options])

阻塞当前进程,直到任意一个子进程退出或者收到一个结束当前进程的信号,子进程结束发送的SIGCHLD不算。使用$status返回子进程的状态吗,并可以制定第二个参数来说明是否已阻塞状态调用

## IO相关
### sprintf
功能很简单,但是遇到了一个坑,formate字符串中如果包含了需要转义的%,没有转义会爆two few arguments的错误
转义方法 % => %%

## 杂
### 不定数量的参数的函数的写法
```PHP
function myfun() {  
	$numargs = func_num_args();  
	echo "参数个数: $numargs\n";  
	$args = func_get_args();//获得传入的所有参数的数组  
	var_export($args);  
}    
```
#### func_num_args
>获取当前函数的传参的个数

#### func_get_args
>获取当前函数的传参数组

### unpack/pack



