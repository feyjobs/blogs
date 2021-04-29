---
title: maven
date: 2021-04-19 11:22
categories: 
tags: 
---

### dependencyManagement && dependencies
dependencyManagement不进行实际的依赖引入而是一个版本管理工具
```xml
<dependencyManagement>
	<dependencies>
		<dependency>
			<groupId>org.projectlombok</groupId>
			<artifactId>lombok</artifactId>
			<version>1.18.10</version>
			<scope>provided</scope>
		</dependency>
	</dependencies>
</dependencyManagement>
```
这种方式不会引入lombok, 仅仅是记录了一个lombok的可用版本, 一般用于父子项目在父项目中定义一些通用依赖的版本

```xml
<dependencyManagement>
	<dependencies>
		<dependency>
			<groupId>org.projectlombok</groupId>
			<artifactId>lombok</artifactId>
			<version>1.18.10</version>
			<scope>provided</scope>
		</dependency>
	</dependencies>
</dependencyManagement>
<dependencies>
	<dependency>
		<groupId>org.projectlombok</groupId>
		<artifactId>lombok</artifactId>
		<scope>provided</scope>
	</dependency>
</dependencies>
```
这种方式就可以在依赖管理的*dependencies*中不填写version, 而使用父项目中的version
