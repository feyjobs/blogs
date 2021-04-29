---
title: js vs lua
date: 2020-10-05 10:01
categories: 
tags: 
---

最近在学习javascript, 感觉js这个语言和lua有一丢丢相似所以把两个语言对比下

# 变量
## 申明
javascript
```javascript
{
    //作用域
}

```

lua
作用域, 用一个
```lua
do
   --code
end
```
代码块表示一个作用域,其他如函数, if, 循环等控制块也自成一个作用域

*local*关键词表示该变量只在当前作用域生效, 不带*local*关键词则表示全局生效
```lua
do
    local a = 12 //本地
    b = 13       //全局
end
```

