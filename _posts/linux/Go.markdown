---
title: Go
date: 2018-04-01 22:56
categories: 
tags: 
---
# 基础
## package
Go引入包,可以通过路径引入,包名为路径最后元素
```go
import (
    "math/rand"  //包名为rand 调用方法为rand.xxxxx
)
```

************************************
package内变量,只有以首字母开头才能在包外被读取
```
import (
    "math"
)

pi:=math.Pi
//a :=math.pi 无法访问 报错
```

## Functions
go鬼畜的函数申明方式
``` go
func func_name(argu1,argu2 type1,argu3,argu4 type2) type3{

}
```
**************************************
go函数返回能返回好多东西,这个貌似不错,不用天天造数组了
```go
func love() (int, int){
    return 1,21
}

a,b := love()
```
*************************************
go函数返回值能在函数声明中预定义,并且在函数中直接使用,如果在return语句中没有带上返回值,则默认返回声明的变量.
```go
func love(a,b int)(x,y int){
    x=a
    y=b
    return
}
```

## 变量
var关键词定义变量,可以在package级别也可以在func级别
************************************
在函数内部,可以使用**:=**关键词,隐式申明一个变量,此时变量类型可以不用申明,根据传的值来确定
```go
func love(){
    a := 2
}
```
但是注意在函数外部,任何变量都需要有var,func等关键词申明,否则编译不会通过
************************************
基本类型:
```go
bool

string

int  int8  int16  int32  int64
uint uint8 uint16 uint32 uint64 uintptr

byte // alias for uint8

rune // alias for int32
     // represents a Unicode code point

float32 float64

complex64 complex128 
```
类似import引入一般,var也可以用()将申明的变量括起来
```go
var (
	ToBe   bool       = false
	MaxInt uint64     = 1<<64 - 1
	z      complex128 = cmplx.Sqrt(-5 + 12i)
)
```
如果var申明变量没有被初始化,那么变量会更具类型,设置一个初值
>0 for numeric types,
>false for the boolean type, and
>"" (the empty string) for strings.
************************************
类型转化
type(value)即可
```go
var a float = float(b)
d := float(c)
```
和C语言不同,go在进行类型转化时,不需要进行显示类型申明
```
var a = float(b)
```
************************************
通过 := 或者说 var方式未进行类型定义的变量,他们的类型值都是通过,右边的值得类型,或者
说右边返回值的类型来进行确定的

************************************
const变量
* 使用const变量申明
* const变量无法使用:=操作符申明
* const变量类型可以是数字,字符串
# 控制
## 循环
```go
for init;boolean;post{
}
```
和其他语言没啥区别,for后面没有()把初始值,和判断条件等框起来,{}是必须的
init和post是可以木有滴,把init和post都去掉
```go
for bool {
xxx
}
```
就和其他语言的while循环一样一样的了
boolean也是可以木有滴,写个死循环就是easy
```go
for{
}
```
## 条件
```go
if statment;bool {
}else{
}
```
和for一样,条件判断不需要(),但是{}是必须的
statment申明的变量的作用范围在if,以及该if对应的else作用范围内

### switch
```go
switch statment;vari{
	case xx:
		xxx
	case xxx:
		xx
	default
}
```
switch case和其他语言一样,但是不需要在每个case后面加break nice,从上往下匹配,只要有一个case匹配上了就匹配完成

switch后不加条件默认跟true,可以极大方便简化很多if else语句

### defer
指向一个函数,该函数会在,该声明所在的代码块执行完之后执行
defer函数多次声明,为stack关系,先入函数后执行

## 高级数据结构
### 指针
和C语言一样,指针指向一个值得内存地址
和C语言一样,&操作符获取一个值得内存地址
和C语言一样,*操作符获取某个指针指向的内存的内容
和C语言**不一样**,Go指针不支持计算

### 结构体
声明方式
```go
type name struct{
    vari   type1
    vari2   type2
}
```
结构体访问方式和C语言一样,**.**访问
name.vari例如

那么结构体指针怎么访问其中的变量呢?
v为结构体
p := &v
(*p).avri可以访问
那么可不可以p->vari呢,**不可以**,Go没有这个选项!,但是可以直接通过指针+.符号访问变量
**p.vari**稳妥
*******************
初始化方式
var (
    v1 = name()
    v2 = name(vari:x,vari:y)
    v3 = name()
    v4 = &name(1,2)
)

### 数组
声明方式
```go
var name [size]tye
```
初始化方式
申明后一个一个赋值
或者
name := [size]type{xx,xx,xx,xx}大括号里元素数量可以和size不一致会用默认类型初始化
### slice
slice是可以视为一个数组的动态视图，在实际使用中比数组来的更加实用
数组方法声明&&初始化方法
```go
var a []T = array[low:high]
```
* **注意,low,high都是数组下标，而且l包含low，不包含high 真撒比**
* low,high在初始化的时候都可以不填，都会默认上下届
> slice不存储任何数据，slice仅仅是一个数组的视图，你对slice所做的任何修改都会造成数组的修改，以及相对应的该数组其他slice元素的修改

自定义声明&&初始化方法
```go
r := []bool{true,false,true,false}
```

* len() slice含有元素的多少
* cap() 从slice第一个元素算起，array与多少个元素
* 空slice nil
## 接口&&方法
### method
method是啥,一种特殊的函数,这种函数在声明时带上了receiver
```go
func (receiver Type)funcName() () {

}
```
这样在方法内可以访问receiver,receiver调用方法也可以直接用receiver.funcName的方法


## 并发(貌似是go语言最主要的特性)
```go
go f(x,y,z)
```
就会新开一个协程运行这个f(x,y,z)函数,所有协程运行于相同的地址空间,所以访问同一块内存时必须保证是同步的
**sync** package提供了有用的相关功能,虽然你可能是不会经常用到,毕竟还有其他更常用的

### channels
channels是一种通道数据结构,允许你通过**<-**操作符放入数据,取出数据
```go
ch <- v   //v放入ch
v <- ch   //从ch取出数据赋值给v
```
**********************************
channels申明初始化方法
```go
ch := make(chan int)
```
以上声明方法为不带缓冲区的channels声明方法,程序执行的时候回阻塞在读和写channels的调用处
ch<- -<ch **读写管道都会阻塞直到另一边执行完成**
#### 有缓冲channels
声明方法
```go
ch := make(chan int,size)
```
有缓冲就很好理解了,**在缓冲没满的情况下,发送方就不会阻塞,在缓冲区没空的情况下,接收方就不会阻塞**

#### close && range
**发送方**可以选择在适当时候close一个channel
range ch 遍历channel中所有元素,直道遇到close
```go
a,ok := <-ch
//ok返回bool值,判断当前channel是否被close
```
#### select
select表达式有点像switch case语句
```go
select {
case xxxx:
    xxx
case xxxx:
    xxx
default:
    xxxxx
}
```
如果两个都可以执行,随便挑一个,好过分
### sync.mutex
sync包提供了一个变量sync.mutex锁可以对










