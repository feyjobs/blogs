---
title: 重构 改善既有代码的设计
date: 2017-07-12 10:34:10
categories: 
- Code
- Read
---

## 重构原则
### 何为重构
> 在软件内部结构的一种调整,目的是在不改变软件可观察行为的前提下,提高其可理解性,降低其修改成本
> 使用一系列重构手法,在不改变软件可观察行为的前提下,调整其结构

- 重构目的:是软件更容易被理解和修改(往往与性能优化相反,性能优化往往会使代码更难理解)
- 重构不改变软件可观察行为

#### 两顶帽子
两顶帽子:**添加新功能** **重构**
无论何时都要清楚自己带的是那一顶帽子,一定不要都带上!!

### 为何重构
1. 重构改进软件设计
	- 代码维持自己该有的形态
	- 消除重复代码
2. 使软件更容易理解
	- 使他人更容易理解一份代码
	- 使自己理解当前代码
3. 找bug
	- 重构可以帮忙找出现存逻辑的bug
4. 提高编程速度
	- 良好的设计是维持软件开发速度的根本
	- 重构能阻止系统腐败, 提高系统的设计质量

### 何时重构
#### 三次法则
	事不过三,三则重构
#### 添加功能时
	- 加强原代码理解
	- 修改源代码使之增强可拓展性,从而使添加新功能更加轻松

#### 修补错误时
#### Code Review的时候
	
### 怎么跟经理说
- 讲道理的,告诉他,然后排期
- 不讲道理的,不告诉他,暗暗排期

### 重构的难题
#### 数据库
#### 修改接口
 - 让旧接口调用新接口。同时维护两个接口
 - 不要发布接口
 
#### 难以通过重构手法完成的设计改动
 1. 设计之前考虑重构方案
 2. 存在=> 选择最简单的设计方案
 3. 找不到=> 好好设计别出坑

#### 何时不该重构
 - 现存代码无法运作
 - deadline不重构
 
### 重构与设计



## 代码的坏味道
### 重复代码
1. 两个函数有相同代码块,Extract method
2. 兄弟子类包含相同的表达式,推到父类去

### 过长函数
> 端函数利益:解释能力,共享能能力,选择能力

**每当感觉需要以注释来说明点什么的时候,我们就需要吧说明的东西扔进一个独立的函数中,并以其用途命名**

#### 



### 重构列表
#### 记录格式
1. 名称
2. 概要
3. 动机
4. 做法
5. 范例

### 重新组织函数
最大问题:过长函数
Extract Method

问题点:
	- 局部变量(临时变量)
	- 参数问题

#### Extract Method
动机:
>一个函数多长合适:关键在于,函数名称和函数本体指间的语义距离

短函数的优势:
1. 粒度小,容易被复用
2. 粒度小,函数名解释充分
3. 函数覆写更容易

做法:
	- 创建新函数(函数名/做什么 不是怎么做)
	- 提炼代码复制到新函数中
	- 检查是否有局部变量

#### Introduce Explaining Variable
> 你有一个复杂的表达式。
> 将该表达式的结果放进一个临时变量,用这个变量的名称来解释表达式的用途

##### 动机:
	解释难懂表达式,但是作者并不推荐= =,因为他觉得不如来个函数实在,还能给别人用= =。除非有很多局部变量不好拆解,不然建议用Extract Method

##### 做法:
	- 声明final临时变量
	- 将表达式的运算结果这一部分,替换
	- 编译,测试

#### Splite Temporary Variable
> 你的程序有摸个临时变量复制超过一次,它既不是循环变量,也不是用于收集计算结果。**针对每次复制,创建一个独立,对应的临时变量**

##### 动机
加强可读性,一个变量多次复制让人糊涂

##### 做法
- 修改第一个待分解临时变量,重命名
- 将新的临时变量声明为 final
- 以第二次赋值动作为界,修改此前对该临时变量的应用点
- 在第二次赋值处,重新声明原变量
- 编译 测试


#### Remove Assignments to Parameters
> 代码对一个参数进行赋值,用临时变量替换改参数












