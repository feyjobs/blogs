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

