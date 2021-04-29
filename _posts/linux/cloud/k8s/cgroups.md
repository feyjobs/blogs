---
title: docker-intro
date: 2020-11-04 10:50
categories: 
tags: 
---

昨天简单了解了下namespace的原理, namespace的主要作用是做隔离, 而docker的另一个非常重要的基石就是cgroups, 今天就来挖一下cgroups的基本原理,
博客内容主要来自[Cgroups, namespaces, and beyond: what are containers made from?](https://www.youtube.com/watch?v=sK5i-N34im8&t=53s)

# what is container
