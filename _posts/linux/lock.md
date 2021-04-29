---
title: lock
date: 2020-04-02 15:04
categories: 
tags: 
---
锁是多线程编程解决并发冲突非常重要的手段, 但是这些概念脑子里依然突出一个混乱, 所以需要梳理一下。
以下内容仅针对c/c++, linux环境。一些与锁相关的内容也会一起讨论。

# 多线程
linux环境下, 一般来说使用的多线程库是pthread(还有一种NPTL), 
c++11以后提供了对线程语言级别的封装(在linux其实就是对pthread做了一层封装, 使用这个库甚至还需要-lpthread, 不考虑跨平台甚至都不需要使用c++11的多线程)。
## pthread
```c
#include <pthread.h>
//创建线程
int pthread_create(pthread_t * thread, const pthread_arrt_t* attr,void*(*start_routine)(void *), void* arg);
//终止线程
int pthread_cancel(pthread_t thread);
//线程等待, 当前线程等待thread线程执行完毕
void pthread_join(pthread_t thread,void ** retval);
```

一个典型的pthread使用方法
```c
#include <pthread.h>
#include <stdio.h>

int sum = 0;

void* count(void* arg){
    int n = *(int*)arg;
    while(n > 0){
        sum++;
        n--;
    }
    return arg;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int i = 1000000;
    int j = 1000000;
    int* ans;
    pthread_create(&thread, &attr, &count, &i);
    while(j > 0){
        sum++;j--;
    }
    pthread_join(thread,(void**)&ans);
    printf("ans: %d\n", *ans);
    printf("sum: %d\n", sum);
    return 0;
}
```

## c++11 thread
c++11引入了多线程编程的相关内容，主要是一个thread类以及相关操作,下面是一个demo
```c++
#include <iostream>
#include <thread>
#include <string>

using namespace std;
int sum = 0;
void threadProc(int seconds){
    for(int i = 0; i < seconds; i++){
        sum++;
    }
}

int main(){
    thread t1(threadProc, 1000000);
    thread t2(threadProc, 1000000);
    t1.join();
    t2.join();
    cout << sum << endl;
    return 0;
}
```
记得-lpthread

# 信号量
从上面的demo实际运行可以可以非常easy得得到接过sum!=2000000, 因为两个线程竞争访问sum全局变量, 导致了资源冲突
这里引入第一种解决方法, *信号量*

非常简单的api,提供了一组初始化&&lock&&unlock的api。
```c
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

int sum = 0;
sem_t  sem;

void* count(void* arg){
    int n = *(int*)arg;
    while(n > 0){
        sem_wait(&sem);
        sum++;
        n--;
        sem_post(&sem);
    }
    return arg;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sem_init(&sem, 0, 1);
    int i = 1000000;
    int j = 1000000;
    int* ans;
    pthread_create(&thread, &attr, &count, &i);
    while(j > 0){
        sem_wait(&sem);
        j--;
        sum++;
        sem_post(&sem);
    }
    pthread_join(thread,(void**)&ans);
    printf("ans: %d\n", *ans);
    printf("sum: %d\n", sum);
    return 0;
}
```
需要指出的是, 陈硕大佬的linux多线程服务端编程,专门有一节点艹信号量和读写锁, 表示这俩没啥用。理由是
* 信号量完全可以用锁+条件变量替代
* 信号量自己在内存中存储了计数，而一般来说用户的应用程序也会存储, 造成浪费

# 锁
## pthread系列
pthread库提供了一系列线程同步函数,大致得抄一下书
```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr); //初始化
int pthread_mutex_destroy(pthread_mutex_t *mutex); //销毁

int pthread_mutex_lock(pthread_mutex_t *mutex); //上锁
int pthread_mutex_trylock(pthread_mutex_t *mutex); //尝试上锁, 如果已经被上锁了, 就返回失败, 相比上面得,优点在不会阻塞
int pthread_mutex_unlock(pthread_mutex_t *mutex);//解锁
```
参考demo
```c
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

int sum = 0;
pthread_mutex_t lock;

void* count(void* arg){
    int n = *(int*)arg;
    while(n > 0){
        pthread_mutex_lock(&lock);
        sum++;
        n--;
        pthread_mutex_unlock(&lock);
    }
    return arg;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_mutex_init(&lock, NULL);
    int i = 1000000;
    int j = 1000000;
    int* ans;
    pthread_create(&thread, &attr, &count, &i);
    while(j > 0){
        pthread_mutex_lock(&lock);
        j--;
        sum++;
        pthread_mutex_unlock(&lock);
    }
    pthread_join(thread,(void**)&ans);
    printf("ans: %d\n", *ans);
    printf("sum: %d\n", sum);
    return 0;
}
```

### pthread spin lock
pthread系列还提供了一套spin lock用于线程同步,所谓spin lock就是当线程尝试抢锁时如果失败, 会一直尝试而不进入阻塞状态
```c
#include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);

int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
```
基本和上面的api一样一样的，注意下init函数的pshared可以设置PTHREAD_PROCESS_SHARED or PTHREAD_PROCESS_PRIVATE,意思是是否允许其他进程的线程访问这个lock
demo
```c
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

int sum = 0;
pthread_spinlock_t lock;

void* count(void* arg){
    int n = *(int*)arg;
    while(n > 0){
        pthread_spin_lock(&lock);
        sum++;
        n--;
        pthread_spin_unlock(&lock);
    }
    return arg;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_spin_init(&lock, PTHREAD_PROCESS_SHARED);
    int i = 1000000;
    int j = 1000000;
    int* ans;
    pthread_create(&thread, &attr, &count, &i);
    while(j > 0){
        pthread_spin_lock(&lock);
        j--;
        sum++;
        pthread_spin_unlock(&lock);
    }
    pthread_join(thread,(void**)&ans);
    printf("ans: %d\n", *ans);
    printf("sum: %d\n", sum);
    return 0;
}
```

## 自己实现系列
在研究nginx的时候发现, nginx进程间通信并没有使用pthread系列提供的锁, 而是非常硬核得自己实现了原子操作,对锁进行了封装
下面小小描述下nginx实现锁的

首先要有锁, 我们要有一个互斥的内容, 简单起见, 我们就规定一个特殊的int类型为原子类型
```c
typedef int atomic_t;
```

确定了类型, 我们就需要对这个类型进行一系列操作的封装啊, 不然普通的+-*\依然后会线程冲突啊.
这里取+作为例子
```c
#define NGX_SMP_LOCK  "lock;"
static ngx_inline ngx_atomic_int_t
ngx_atomic_fetch_add(ngx_atomic_t *value, ngx_atomic_int_t add)
{
    __asm__ volatile (

         NGX_SMP_LOCK
    "    xaddq  %0, %1;   "

    : "+r" (add) : "m" (*value) : "cc", "memory");

    return add;
}
```
这里使用在c语言内内嵌汇编代码来实现
__asm__是一个gcc提供的功能后面可以追加汇编代码
volatile关键词的意义是:告诉编译器, 不要优化这块代码, 就按照我写的运行!

不想看的汇编生成的的小伙伴可以直接gcc -S -O0 生成代码相应的汇编代码查看这段汇编最终形态
```asm
	lock;    
    xaddl  %eax, (%rdx);
```
总的来说两句话, 第一个指令lock,锁住cpu总线, 现在开始只有我可以访问内存!

第二个函数 value值会进入rdx寄存器, add进入eax寄存器, 非常简单 xaddl从rdx寄存器取地址,并获取内存中的内容, 加上eax寄存器中的临时值,并写回内存
所以！ 这块功能的核心在于**lock;**指令, 这个指令会指定下一个指令运行期间锁住总线, 其他cpu访问内存都会失败, 所以这个才是锁在操作系统层面的实现原理, 至于x86怎么实现的
告辞。至此我们有理由可以猜测, 操作系统的锁相关的核心都是这个lock;指令以及xaddq这类单命令完成读写内存操作的指令, 只是有些锁在竞争中失败后阻塞睡眠了, 有些锁
锲而不舍的一直尝试加锁, 这个层面就是cpu空跑与线程状态切换的成本博弈了。

想琢磨怎么这个汇编怎么生成的小伙伴可以参考下面的链接帮助理解
参考资料: [gcc内联汇编](https://www.cnblogs.com/xphh/p/11491489.html)
## 杀鸡用牛刀
```c
#include <stdio.h>
int sum = 0;
static inline int atomic_fetch_add(int* value, int add){
    __asm__ volatile (
            "lock;"
    "    xaddl  %0, %1;   "

    : "+r" (add) : "m" (*value) : "cc", "memory");
    return add;
}

void* count(void* arg){
    int n = *(int*)arg;
    while(n > 0){
        atomic_fetch_add(&sum, 1);
        n--;
    }
    return arg;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int i = 1000000;
    int j = 1000000;
    int* ans;
    pthread_create(&thread, &attr, &count, &i);
    while(j > 0){
        atomic_fetch_add(&sum, 1);
        j--;
    }
    pthread_join(thread,(void**)&ans);
    printf("ans: %d\n", *ans);
    printf("sum: %d\n", sum);
    return 0;
}
```
这上面我们自己封装了原子操作，实现了锁功能










