---
title: LRUCache
date: 2020-12-02 14:30
categories: 
tags: 
---
leveldb的LRUCache实现, 没啥好说的, 基本可以认为是标准答案了吧

## 使用篇
```c++
Cache* NewLRUCache(size_t capacity); //创建cache, capacity容量, 词条数
Handle* Insert(const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value)); //插入一个kv
Handle* Lookup(const Slice& key); // 搜索一个key
void Release(Handle* handle); // 释放当前kv的句柄, 这个kv不一定会被删除, 说不定被别的线程还有这个kv的句柄
```
