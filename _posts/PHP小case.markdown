---
title: PHP小case
date: 2017-02-03 17:43:10
categories: 
- Code
- PHP
---
#### 闭包
PHP闭包函数如果要使用外部变量主要要加上 use关键词/(ㄒoㄒ)/~~,又特么浪费时间
```PHP
      int $sky = 1;
    array_filter($fairys,function love($fairy) use($sky) {
        echo $sky;
    });
```
不加use在闭包函数内,这个$sky就是个NULL.

还是闭包= =。好蠢 
注意:如果要写一个

#### tpl
一个上古老古董,百度居然还在用,我老天。
介绍下,首先这是个前后端分离用的,后端通过tpl的形式提供数据,然后通过模板整合页面与数据,然后一起生成一个页面

#### 二维数组求某列的和
稳妥
```PHP
	function getOneColSumInTwoDimArray($arr, $col) {
		$func = function($value) use($col) {
			return $value[$col];
		};
		return array_sum(array_map($func, $arr));
	}
```
#### json_encode
在对一个数据进行json_encode的时候一定要确保,数组内不存在NAN数据类型,json无法解析会直接崩溃

#### 空格也能做php数组的key
好傻逼= =
```PHP
$array = array(' '=> 12, '  '=> 12);
```
是有意义的,where条件写习惯了,习惯性在两边加了空格造成了bug
