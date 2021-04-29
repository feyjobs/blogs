---
title: epoll
date: 2020-04-10 23:27
categories: 
tags: 
---

<p>
epoll事件机制是linux下高性能服务器的核心战斗力, nginx envoy等服务器也在linux下使用了该技术。
首先简单说一下这个技术是什么，解决什么问题, 众所周知高并发服务器需要处理大量连接请求, 来来往往的tcp, udp连接络绎不绝
为了能处理这么多的请求, 一般有几种方法, 多线程，异步化。但是线程是不可能无限制得创建的, 毕竟cpu核数是固定的，真的来一个连接就创建一个线程处理,
势必造成线程数大大超过核数, cpu处理不过来，造成频繁的线程状态切换, 浪费cpu性能。
而异步化呢, 就是将一个链接中所有能够异步处理的操作都异步化, 异步读, 异步写, 那么既然异步化了, 怎么知道有没有信息可以读, 写完了没有呢
这就是IO多路复用的作用了, epoll就是其中一种io多路复用机制, 我们可以将读写事件注册在epoll上, 当有相应的操作触发时, epoll就会通知我们进行相应的处理，
从而避免了在读写操作上的阻塞
<p>

## 使用                  
epoll的核心api只有三个
```c
#include <sys/epoll.h>
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd,struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
```
核心数据结构一个
```c
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  /**
  * 表示该event关注的事件, 就是epoll_ctl第三个参数fd可能发生的事件
  * EPOLLIN  读事件
  * EPOLLOUT 写事件
  * EPOLLPRI  
  * EPOLLERR fd错误
  * EPOLLHUP fd挂断
  * EPOLLONESHOT 只出发一次epoll事件
  * EPOLLET 边缘触发模式
  */
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;
```

### epoll_create
创建一个epoll对象返回epfd,用于epoll_ctl, epoll_wait, size参数表示这个epoll对象监听的fd数, 在高版本里这个参数已经没有意义, 系统会自动根据添加的fd动态分配内存,
但是还是需要设置成非负数

### epoll_ctl
操作一个epfd, 操作的动作根据第二个参数确定
op参数的作用
* EPOLL_CTL_ADD 注册一个fd的事件(event)到该epoll实例上去, event描述fd的事件类型
* EPOLL_CTL_MOD 编辑一个fd的event
* EPOLL_CTL_DEL 删除该fd, event可以为空

### epoll_wait
获取epfd上触发的事件列表, 通过第二个参数events返回, maxevents表示最多返回多少event, timeout表示epoll_wait至少等待几秒, -1 一直等待， 0 不等待，该函数返回值表示获取的event数

### demo
```c
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#define nullptr (void*)-1;


int main(){
    struct sockaddr_in servaddr;
    struct sockaddr addr;
    int flags;
    int epfd;
    int cnt;
    int i;
    int n;
    int sfd;
    char buf[1024];
    socklen_t len;
    struct epoll_event event, *ee;
    struct epoll_event* events = (struct epoll_event*)malloc(sizeof(struct epoll_event)* 1024);

    //监听端口, 启动服务
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1024);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd: %d\n", sockfd);
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags| O_NONBLOCK);
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    perror("bind");
    listen(sockfd, 1024);
    perror("listen");

    //创建epoll fd, 将上面的监听socket放进epoll
    epfd = epoll_create(1024);
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

    while(1){
        //获取当前的已发生的事件列表
        cnt = epoll_wait(epfd, events, 1024, 10*1000);
        if (cnt > 0){
            printf("%d events got\n", cnt);
            for(i = 0; i < cnt; i++){
                ee = &events[i];
                sfd = ee->data.fd;
                if (ee->events & EPOLLRDHUP) {
                    printf("%d: EPOLLRDHUP occurred\n", sfd);
                }

                if (ee->events & EPOLLIN) {
                    printf("%d: EPOLLIN occurred\n", sfd);
                }
                //判断是不是监听套接字触发了, 如果是的话就要accept, 将链接套接字放进epoll
                if (sfd == sockfd){
                    printf("got a listen socket \n");
                    sfd = accept(sockfd, &addr, &len);
                    if (sfd != -1){
                        event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
                        event.data.fd = sfd;
                        flags = fcntl(sfd, F_GETFL, 0);
                        fcntl(sfd, F_SETFL, flags| O_NONBLOCK);
                        epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
                        continue;
                    }
                    printf("accept failed!\n");
                }
                //如果是链接套接字触发了, 则读取套接字中的缓冲区内容
                n = recv(sfd, buf, 1024, 0);
                if (n > 0){
                    buf[n] = 0;
                    printf("%d: %s \n",sfd, buf);
                    memset(buf, 0, 1024);
                }
                //如果缓冲区返回0字节, 则表示对端关闭了链接, 这边也关闭链接, 将事件删除
                if (n == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sfd, 0);
                    close(sfd);
                    printf("close fd %d \n", sfd);
                }
            }
        }
        printf("did not recv any events, errno: %d\n",errno);
        sleep(10);
    }
    return 0;
}
```
简单对上面demo做一个总的解释, 服务会启动一个监听套接字监听tcp链接的到来, 对每一个tcp链接都会创建一个链接套接字, 进行这个链接上的读写
![demo1](/uploads/nginx/epoll/demo1.jpg)

### EPOLLET
这个epoll_ctl的event类型必须仔细聊一聊, 上面的说明里, 写了设置这个选项后, 这个事件将会以边缘模式触发, 怎么解释呢。
![square](/uploads/nginx/epoll/square.jpg)
途中红线部分表示边缘, 绿线部分表示水平, epoll在默认情况下是水平触发的什么意思, 这要这个事件还存在, 他就会一直epoll_wait返回,
而边沿触发呢, epoll_wait返回之后, 如果没有新的事件到来，epoll_wait就不会继续返回这个事件, 比方说有个链接到了, 我们epoll_wait读到了事件
但是没有accept创建链接, 那么除非有新的链接事件到来, 否则epoll_wait就不会再返回这个事件了, 边缘触发可以通过一个简单的修改演示
```c
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#define nullptr (void*)-1;


int main(){
    struct sockaddr_in servaddr;
    struct sockaddr addr;
    int flags;
    int epfd;
    int cnt;
    int i;
    int n;
    int sfd;
    char buf[1024];
    socklen_t len;
    struct epoll_event event, *ee;
    struct epoll_event* events = (struct epoll_event*)malloc(sizeof(struct epoll_event)* 1024);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1024);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd: %d\n", sockfd);
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags| O_NONBLOCK);
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    perror("bind");
    listen(sockfd, 1024);
    perror("listen");

    epfd = epoll_create(1024);
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

    while(1){
        cnt = epoll_wait(epfd, events, 1024, 10*1000);
        if (cnt > 0){
            printf("%d events got\n", cnt);
            for(i = 0; i < cnt; i++){
                ee = &events[i];
                sfd = ee->data.fd;
                if (ee->events & EPOLLRDHUP) {
                    printf("%d: EPOLLRDHUP occurred\n", sfd);
                }

                if (ee->events & EPOLLIN) {
                    printf("%d: EPOLLIN occurred\n", sfd);
                }
                if (sfd == sockfd){
                    printf("got a listen socket \n");
                    sfd = accept(sockfd, &addr, &len);
                    if (sfd != -1){
                        //这里指定了边缘触发
                        event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
                        event.data.fd = sfd;
                        flags = fcntl(sfd, F_GETFL, 0);
                        fcntl(sfd, F_SETFL, flags| O_NONBLOCK);
                        epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
                        continue;
                    }
                    printf("accept failed!\n");
                }
                //注意:!! 这里读取缓冲区的内容只读取1字节, 造成读事件没有结束
                n = recv(sfd, buf, 1, 0);
                if (n > 0){
                    buf[n] = 0;
                    printf("%d: %s \n",sfd, buf);
                    memset(buf, 0, 1024);
                }
                if (n == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sfd, 0);
                    close(sfd);
                    printf("close fd %d \n", sfd);
                }
            }
        }
        printf("did not recv any events, errno: %d\n",errno);
        sleep(10);
    }
    return 0;
}
```
![EPOLLETinput](/uploads/nginx/epoll/EPOLLETinput.jpg)
![EPOLLEToutput](/uploads/nginx/epoll/EPOLLEToutput.jpg)
可以看到, 我们输入一串q, 但是事件触发后只读取一个q后, epoll_wait就不再返回剩下的字符串了, 但是如果我们把EPOLLET去掉, 接过如下
![EPOLLLToutput](/uploads/nginx/epoll/EPOLLLToutput.jpg)
每次epoll_wait都返回了fd5的读事件, 并且读出一个字节。

在nginx设计中, 大部分的读写事件都被设置成了EPOLLET模式, 即边沿触发, 唯独监听套接字的读事件采用的水平触发, 因为如果一个链接创建请求被读取, 而没有被accept就必须要等下一个链接进来
才能accept, 为了方式链接创建阶段超时, nginx为了防止这种现象, 将他们设置成了水平触发的模式。


## 惊群
![cats](/uploads/nginx/epoll/cats.jpg)
何为惊群? 一条小鱼干送到小奶猫们脸上, 每只小奶猫都会抬起头嗷嗷待哺, 但是! 只有一条小鱼干, 所以最终也就只有一直小奶猫会吃到小鱼干,
其他小猫咪都白抬头了！！
惊群描述的就是这么一个现象。多个线程同时监听同一个套接字, 当这个套接字来了一个请求时, 他们都齐刷刷accept,但是最终只会有一个线程创建链接成功, 造成了其他线程资源浪费
,于是在某一个上古版本(2.6)中, accept函数的惊群现象终于被修复了!

accept惊群虽然修复了, 然而多个epoll对象监听同一个套接字的链接创建事件, 在epoll_wait时依然会造成惊群
### demo
```c
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#define nullptr (void*)-1;

void run(int epfd, int sockfd, int id){
    int cnt;
    struct epoll_event* events = (struct epoll_event*)malloc(sizeof(struct epoll_event)* 1024);
    struct sockaddr addr;
    socklen_t len;
    int sfd;

    while(1){
        printf("worker %d: enter block\n", id);
        cnt = epoll_wait(epfd, events, 1024, -1);
        printf("worker %d wake up,cnt: %d\n",id,cnt);
        if (cnt == -1){
            perror("epoll_wait");
        }
        if (cnt > 0){
            sfd = accept(sockfd, &addr, &len);
            if (sfd == -1){
                printf("worker %d: waked!,but got a shit\n", id);
                continue;
            }else{
                printf("worker %d: waked!,sfd:%d\n", id, sfd);
                close(sfd);
            }
        }
    }
}

int main(){
    struct sockaddr_in servaddr;
    int flags;
    int i;
    int pid;
    int spid[4];
    int epfd;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1024);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd: %d\n", sockfd);
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags| O_NONBLOCK);
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    perror("bind");
    listen(sockfd, 1024);
    perror("listen");

    struct epoll_event event;
    epfd = epoll_create(1024);
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

    for(i = 0; i < 4;i++){
        if((pid = fork()) == 0){
            run(epfd, sockfd, i);
        }
        spid[i] = pid;
    }

    for(i = 0; i < 4;i++){
        wait(NULL);
    }
    return 0;
}
```
这段程序中, 我启动四个进程监听同一个socket, 然后用telnet进行tcp链接,结果
![jinqun](/uploads/nginx/epoll/jinqun.jpg)
0,2,3号进程被唤醒, 只有3号进程, 而0,2号进程唤醒之后,啥事没有就继续阻塞了, 宝宝心里苦!

如何解决这个问题! **EPOLLEXCLUSIVE**
```c
    struct epoll_event event;
    epfd = epoll_create(1024);
    event.events = EPOLLIN|EPOLLEXCLUSIVE;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
```
在事件初始化中, 加入EPOLLEXCLUSIVE选项, 表示该事件触发不会唤醒所有进程, 然而还是不能完全规避少部分进程被唤醒。
怎么解！

#### 锁
多线程的世界, 哪里有冲突, 哪里就有锁!epoll_wait不解决惊群, 那就用户层上锁解决, 这里截取nginx的惊群解决方法部分逻辑
```c
//判断是否开启accept共享锁
if (ngx_use_accept_mutex) {
    if (ngx_accept_disabled > 0) {
        ngx_accept_disabled--;

    } else {
        //尝试上accept锁, 上了之后, 其他线程就无法调用epoll_wait了
        //抢不到锁就滚蛋
        if (ngx_trylock_accept_mutex(cycle) == NGX_ERROR) {
            return;
        }

        if (ngx_accept_mutex_held) {
            flags |= NGX_POST_EVENTS;

        } else {
            if (timer == NGX_TIMER_INFINITE
                || timer > ngx_accept_mutex_delay)
            {
                timer = ngx_accept_mutex_delay;
            }
        }
    }
}

if (!ngx_queue_empty(&ngx_posted_next_events)) {
    ngx_event_move_posted_next(cycle);
    timer = 0;
}

delta = ngx_current_msec;

//这里是nginx的事件处理逻辑, 在这个函数调用了epoll_wait进行事件处理
//各个不同的事件模型实现了不同的ngx_process_events, 下面的代码段抄录了, epoll的process_event的实现
(void) ngx_process_events(cycle, timer, flags);

delta = ngx_current_msec - delta;

ngx_log_debug1(NGX_LOG_DEBUG_EVENT, cycle->log, 0,
               "timer delta: %M", delta);

ngx_event_process_posted(cycle, &ngx_posted_accept_events);

if (ngx_accept_mutex_held) {
    ngx_shmtx_unlock(&ngx_accept_mutex);
}
```

```c
//由于无关代码太多, 做了删减
static ngx_int_t
ngx_epoll_process_events(ngx_cycle_t *cycle, ngx_msec_t timer, ngx_uint_t flags)
{

    //拿到events列表
    events = epoll_wait(ep, event_list, (int) nevents, timer);

    //遍历events事件处理
    for (i = 0; i < events; i++) {
            //这里nginx为了防止创建链接太过耗时造成其他线程一直拿不到锁, 采用将accept事件放进队列, 先放锁,后建链,提高性能
            if (flags & NGX_POST_EVENTS) {
                queue = rev->accept ? &ngx_posted_accept_events
                                    : &ngx_posted_events;

                ngx_post_event(rev, queue);

            } else {
                rev->handler(rev);
            }
        }

        //其他的一些耗时事件也有一样的处理方式
        if (flags & NGX_POST_EVENTS) {
            ngx_post_event(wev, &ngx_posted_events);

        } else {
            wev->handler(wev);
        }
    }

    return NGX_OK;
}
```
综上, nginx为了解决惊群问题, 选择在用户态加锁, 防止epoll_wait竞争, 同时设计了延迟队列, 加快锁的释放, 突出一个极致!
