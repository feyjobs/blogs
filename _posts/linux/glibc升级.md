---
title: glibc升级.markdown
date: 2018-08-29 14:52
categories: 
tags: 
---
准备学一下apue,但是公司坑爹的centos,glibc版本地得令人发指,差了n多资料终于搞定

碰到的最主要的问题是 npm包循环依赖,导致a包依赖b包,b包依赖a包,解决的方法居然是,两个包同时安装

第二个问题是包重复安装

http://movingon.cn/2017/05/05/CentOS-6-x-%E5%A6%82%E4%BD%95%E5%8D%87%E7%BA%A7-glibc-2-17/
