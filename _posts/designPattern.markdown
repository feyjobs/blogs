---
title: 设计模式
date: 2018-02-11 21:20
categories: 
tags: 
---

 # 创建型模式
 * 类创建型模式使用继承改变被实例化的类
 * 对象创建型模式将实例化委托给另一个对象
 
 创建型对象的意义:系统的演化越来越复杂,重心从一组固定行为的硬编码转移为定义一个较小的基本行为集,这些行为集可以被任意组合成任意更加复杂的行为,这样创建有特定行为的对象要求的不仅仅是实例化一个类

 * 将系统使用了哪些具体的类信息封装起来
 * 隐藏了这些类的实例是如何被创建和放在一期的

迷宫初始化case:
我们初始化一个迷宫,而不考虑玩家的具体行为,仅仅做初始化,在这个case中首先声明有以下基本组件
*  房间(room)
*  墙(wall)
*  门(door)

首先有一个组建基类
```c++
class MapSite {
    public :
        virtual void Enter() = 0;
};
```
其余的三个组建都要继承这个类

room类
```c++
class Room : public MapSite {
    public:
        Room(int roomNo);

        MapSite* GetSide(Direction);
        void SetSide(Direction, MapSite*);

        virtual void Enter();
    private:
        MapSite* _sides[4];
        int _roomNumber;

}
```
wall类
```c++
class Wall : public MapSite {
    public :
        Wall();

        virtual void Enter();
};
```

door类
```c++
class Door : public MapSite {
    public :
        virtual void Enter();
        
        Door(Room* = 0, Room* = 0);
        Room* OtherSideFrom(Room*);

    private :
        Room* _room1;
        Room* _room2;
        bool _isOpen;
};
```
迷宫类
```c++
class Maze{
public :
    Maze();

    void AddRoom(Room*);
    Room* RoomNo(int);

private :
    //

}
```
同时定义迷宫游戏类初始化迷宫地图
```c++
Maze* MazeGame::CreateMaze(){
    Maze* aMaze = new Maze();

    Room* r1 = new Room(1);
    Room* r2 = new Room(2);

    Door* theDoor = new Door(r1,r2);
    aMaze->addRoom(r1);
    aMaze->addRoom(r2);

    r1->setSide(North, new Wall);
    r1->setSide(East, theDoor);
    r1->setSide(South, new Wall);
    r1->setSide(West, new Wall);


    r1->setSide(North, new Wall);
    r1->setSide(East,  theDoor);
    r1->setSide(South, new Wall);
    r1->setSide(West, new Wall);
}
```
ok,这样我们就完成了一个迷宫类的基本实现,仅仅有两个宫格的迷宫就有如此多代码,显然是相当复杂的。我们可以将那一堆setSide在Room初始化的时候默认初始化为墙，会使得代码更加精简,但是这个代码的根本 问题不在于代码的长度,而是在于,迷宫的实际生成都依赖于硬编码,如果需要改变布局就需要,改变这个成员函数,这个不方便代码的重用。


创建型模式可以将这个系统设计的更加灵活,但是不一定为更小,特别是，它们将便于修改定义一个迷宫构件的类

## Abstract Factory
### 意图
提供一个创建一系列相关或者相互依赖的=对象的接口,而无需指定它们具体的类

### 别名
Kit(工具箱)

### 举例
比如说我们要实现一个支持多个画风的用户界面工具包(滚动条,窗口,按钮等),那么我们不应该在具体的窗口组件上编码他的视感风格,否则实例化之后,不方便切换画风。

为了解决这个问题,我们可以定义一个抽象的WidgetFactory类,这个类声明了一个用来创建每一类基本窗口组件的接口。每一个窗口组件有一个抽象类,而具体的子类则实现了窗口组件的特定视感风格,
对于每一个抽象窗口组件类,WidgetFactory接口都有一个返回新窗口组件对象的操作。















