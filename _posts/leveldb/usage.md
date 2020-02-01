---
title: usage
date: 2019-07-11 13:58
categories: 
tags: 
- leveldb
---

# 安装
```shell
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```
为了能够对静态库进行gdb调试,我们在CMakeList.txt文件加入
```cmake
add_definitions("-Wall -g")
```
之后我们就可以进行leveldb编译安装了

cmake运行之后make && make install安装

## 坑
* 安装过程中,系统中有两个g++,使用低版本g++编译了leveldb,高版本编译调用代码后,在链接时出现问题,猜测低版本链接器无法链接高版本文件
* 单步调试中,直接s无法进入函数调试,而是去了个奇怪的地方,不确定,但是使用断点然后c是可以进入目标函数的,原因不明

## 测试
使用以下代码进行测试

```c++
#include <assert.h>
#include <string.h>
#include <leveldb/db.h>
#include <iostream>

int main(){
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options,"testdb", &db);
    assert(status.ok());

    //write key1,value1
    std::string key="key";
    std::string value = "value";

    status = db->Put(leveldb::WriteOptions(), key,value);
    assert(status.ok());

    status = db->Get(leveldb::ReadOptions(), key, &value);
    assert(status.ok());
    std::cout<<value<<std::endl;
    std::string key2 = "key2";

    //move the value under key to key2

    status = db->Put(leveldb::WriteOptions(),key2,value);
    assert(status.ok());
    status = db->Delete(leveldb::WriteOptions(), key);

    assert(status.ok());

    status = db->Get(leveldb::ReadOptions(),key2, &value);

    assert(status.ok());
    std::cout<<key2<<"==="<<value<<std::endl;

    status = db->Get(leveldb::ReadOptions(),key, &value);

    if(!status.ok()) std::cerr<<key<<"  "<<status.ToString()<<std::endl;
    else std::cout<<key<<"==="<<value<<std::endl;

    delete db;
    return 0;
}
```
在文件运行之后出现一个testdb的数据库

