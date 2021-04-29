---
title: effectivelua
date: 2020-05-01 09:08
categories: 
tags: 
---

## 短路求值
```lua
--初始化方法
x=x or v -- if not x then x = v end
```

```lua
--三目运算
a and b or c -- 重要前提是b不是false,也不是nil, 否则a是true, 也是返回c
```
