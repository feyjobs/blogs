---
title:  长连接 短连接 连接服用
date: 2019-05-22 14:51
categories: 
tags: 
---
[toc]

参与互联网项目的小伙伴经常会碰到几个相似的词语,导致概念混淆,今天花了一下午时间认真琢磨了一下各个的区别.
* tcp keepalive
* http keepalive(与此同时经常出现的还有长短连接的考虑) * keepalived
<p>
    keepalived是Linux下一个轻量级别的高可用解决方案。顾在此不表重点讲一下tcp和http的keepalive
</p>

## tcp
首先抛出一个自己以前的错误认知:
> tcp长连接

**tcp没有什么长短连接**,只要不出现网络异常,主动断开,tcp连接可以一直跑下去跑到天荒地老

那么tcp的那一堆keepalive参数是干什么的呢?首先看下有那些keepalive参数
```shell
cd /proc/sys/net/ipv4
ll
```
可以看到有三个keepalive参数
* tcp_keepalive_time
* tcp_keepalive_probes
* tcp_keepalive_intvl

<font color=red>tcp_keepalive_time</font>:一个tcp连接空闲了tcp_keepalive_time秒之后,就要怀疑对方的健康状况了,注意:必须在socket上设置了**SO_KEEPALIVE**选项,才会触发这个机制

<font color=red> tcp_keepalive_probes</font>:怎么确认对方的健康状况呢,发送侦测包(probe表示探针,也是星际二中萌萌的神族农民),如果对方能正确返回,那就说明对面还活着,这个参数描述发送几个侦测包(毕竟不要一棍子打死)

![probe](/uploads/nginx/probe.jpg)

<font color=red> tcp_keepalive_intvl</font>:这个描述每隔多少tcp_keepalive_intvl发送一个侦测包

ok,看到这三个参数的意义,我们就明白了,tcp的keepalive是做tcp**健康检查**的

### 测试
#### 正常状况
首先我们使用nc命令启动一个tcp server
```shell
nc -l 2333
```
监听2333端口
然后我们拿python写一段tcp连接代码
```python
import socket
import time
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) 
s.setsockopt(socket.SOL_TCP, socket.TCP_KEEPIDLE, 20)
s.setsockopt(socket.SOL_TCP, socket.TCP_KEEPINTVL, 1)

s.connect(('127.0.0.1', 2333))

time.sleep(61)
```
代码中分别设置了
* SO_KEEPALIVE 启动tcp健康检查
* TCP_KEEPIDLE 空连接20s进行一次检查
* TCP_KEEPINTVL 每隔1s发送一个检查探针
* 还有一个参数tcp_keepalive_probes取系统默认的
这里是
![probe](/uploads/nginx/tcp_keepalive_probes.jpg)
也就是说,空闲20s后开始发送探测报文,每隔1s发一个最多发2个

用tcpdump抓包
```shell
tcpdump -i lo tcp port 2333
```
启动python
首先是三次tcp握手
![handshake](/uploads/nginx/handshake.jpg)

随后等待20s,第一次探测报文发送,server很快回复ok 表示我还活着
![handshake](/uploads/nginx/first.jpg)

然后他们又很无聊得等了20s,又发了一次探测报文,server再次回复ok
![handshake](/uploads/nginx/second.jpg)

最后他们又很无聊得等了20s,又发了一次探测报文,server再次回复ok
![handshake](/uploads/nginx/third.jpg)

程序退出,挥手再见
![handshake](/uploads/nginx/byebye.jpg)

好的,在这个情况下,client与server都愉快得交换了双发意见,达成了互相都健康得共识,接下来看一个不健康的

#### 异常状况
依然用nc命令启动server,python代码也不变,但是我们需要断网使用iptables命令
具体步骤
* 启动tcpddump
```shell
tcpdump -i lo tcp port 2333
```
* nc启动server
* 运行python 完成握手
* **重要** tcp握手之后快速完成端口屏蔽
```shell
iptables -I INPUT -p tcp --dport 2333 -s 127.0.0.1 -j DROP
```
屏蔽127.0.0.1:2333的网络连接,然后就等着下面这一幕
- 兄弟 你还好吗
![handshake](/uploads/nginx/anzhong.jpg)
- +1s 兄弟 你还好吗
![handshake](/uploads/nginx/anzhong.jpg)
- 死了,发送**rst**报文,表示异常终止
![handshake](/uploads/nginx/failed.jpg)

最后
```shell
iptables -F
```
恢复机器路由状况

## http
http模式下的keepalive是用来保证tcp连接复用的.
在最开始的http协议中,并没有关于长连接的概念,单纯得有一个请求就建立一个tcp连接,后来人们发现太浪费了,两个相同终端,发100个请求就会建立100个tcp连接,于是诞生了http长连接keep-alive

### http 1.0 && 1.1
在1.0版本http协议客户端就可以通过加入
```http
Connection: keep-alive
```
头部发起一个长连接请求,服务器在收到之后如果自身也支持长连接,那么就会在返回体重也返回这个头部,表示支持长连接
如果server不支持长连接就可以返回
```http
Connection: close
```
头部表示不支持长连接

版本更新到1.1之后,http请求默认就是长连接,除非在客户端请求中加上
```http
Connection: close
```
关闭长连接

### 性能
我们以nginx为例,测试http长短连接的性能对比

我们在nginx配置文件中加入配置
```nginx
keepalive_timeout  65;
```
这个命令表示,在处理一个http请求后,65s内没有新的请求过来,我们就关闭当前连接

我们使用wrk作压测(一个非常不错的http压测工具)
```
wrk -c 100 -d 10  -t 12 https://localhost:8233
```
上述命令表示起100个连接 12个线程压10s

我们在nginx日志内容中加入**$connection_requests**变量,这个变量表示有多少个请求在和当前请求复用这个连接
![nginxconf](/uploads/nginx/nginxconf.jpg)

#### 长连接压测
![nginxconf](/uploads/nginx/fuyong.jpg)
打印的第一个变量是当前请求的$connection_requests,可以看到所有连接都在处理多个请求

wrk压测结果如下
![nginxconf](/uploads/nginx/yacegood.jpg)

#### 短连接压测
![nginxconf](/uploads/nginx/notfuyong.jpg)
可以看到每个请求的当前连接都只处理一个请求

wrk压测结果如下
![nginxconf](/uploads/nginx/yacebad.jpg)
对比长连接的压测结果,可以看到
qps和tps都相差了**12倍**左右,提升效果非常明显

### 问题
那么长连接还有什么优化的点吗?有的
最明显的地方就是,一个长连接上的所有请求都是串行执行的
也就是说 第一个请求没有执行完,就不会执行第二个请求,再极端一点
客户端需要处理10个请求,但是只开了6个连接,剩下的那4个就要干等着!怎么办！

![nginxconf](/uploads/nginx/kqqrlfz.jpg)

**<font size=12>http2</font>**

## nginx keepalive实现
keepalive的关键就是在请求结束的时候不要断开tcp连接,而是继续等待,下一个请求的到来
```c
void
ngx_http_finalize_request(ngx_http_request_t *r, ngx_int_t rc)
{
    ...
    if (rc == NGX_DONE) {
        /**
        *调用ngx_http_finalize_connection释放连接资源
        */
        ngx_http_finalize_connection(r);
        return;
    }
    ...
}
static void
ngx_http_finalize_connection(ngx_http_request_t *r)
{
    ...
    /**
    *如果没有收到退出命令,并且设置了长连接,长连接未超时,则进入设置keepalive逻辑
    */
    if (!ngx_terminate
         && !ngx_exiting
         && r->keepalive
         && clcf->keepalive_timeout > 0)
    {
        ngx_http_set_keepalive(r);
        return;
    }
    ...
}

static void
ngx_http_set_keepalive(ngx_http_request_t *r)
{
  int                        tcp_nodelay;
    ngx_buf_t                 *b, *f;
    ngx_chain_t               *cl, *ln;
    ngx_event_t               *rev, *wev;
    ngx_connection_t          *c;
    ngx_http_connection_t     *hc;
    ngx_http_core_loc_conf_t  *clcf;

    c = r->connection;
    rev = c->read;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0, "set http keepalive handler");

    if (r->discard_body) {
        r->write_event_handler = ngx_http_request_empty_handler;
        r->lingering_time = ngx_time() + (time_t) (clcf->lingering_time / 1000);
        ngx_add_timer(rev, clcf->lingering_timeout);
        return;
    }

    c->log->action = "closing request";

    hc = r->http_connection;
    b = r->header_in;

    /**
    *pipeline 请求特殊处理
    *pipeline请求就是,客户端在收到返回前先把下一个请求发送给服务端
    *这里把缓存的pipeline请求拿出来
    */
    if (b->pos < b->last) {

        /* the pipelined request */

        if (b != c->buffer) {

            /*
             * If the large header buffers were allocated while the previous
             * request processing then we do not use c->buffer for
             * the pipelined request (see ngx_http_create_request()).
             *
             * Now we would move the large header buffers to the free list.
             */

            for (cl = hc->busy; cl; /* void */) {
                ln = cl;
                cl = cl->next;

                if (ln->buf == b) {
                    ngx_free_chain(c->pool, ln);
                    continue;
                }

                f = ln->buf;
                f->pos = f->start;
                f->last = f->start;

                ln->next = hc->free;
                hc->free = ln;
            }

            cl = ngx_alloc_chain_link(c->pool);
            if (cl == NULL) {
                ngx_http_close_request(r, 0);
                return;
            }

            cl->buf = b;
            cl->next = NULL;

            hc->busy = cl;
            hc->nbusy = 1;
        }
    }

    /* guard against recursive call from ngx_http_finalize_connection() */
    r->keepalive = 0;
    //释放当前请求
    ngx_http_free_request(r, 0);

    c->data = hc;

    if (ngx_handle_read_event(rev, 0) != NGX_OK) {
        ngx_http_close_connection(c);
        return;
    }

    wev = c->write;
    wev->handler = ngx_http_empty_handler;

    /**
    *如果下一个请求已经被收到了,就直接进入下一个请求的开始处理
    *函数,将 读事件handler设置为 ngx_http_process_request_line,开始下一个请求
    */
    if (b->pos < b->last) {

        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0, "pipelined request");

        c->log->action = "reading client pipelined request line";

        r = ngx_http_create_request(c);
        if (r == NULL) {
            ngx_http_close_connection(c);
            return;
        }

        r->pipeline = 1;

        c->data = r;

        c->sent = 0;
        c->destroyed = 0;

        if (rev->timer_set) {
            ngx_del_timer(rev);
        }

        rev->handler = ngx_http_process_request_line;
        ngx_post_event(rev, &ngx_posted_events);
        return;
    }

    /*
     * To keep a memory footprint as small as possible for an idle keepalive
     * connection we try to free c->buffer's memory if it was allocated outside
     * the c->pool.  The large header buffers are always allocated outside the
     * c->pool and are freed too.
     */

    b = c->buffer;

    if (ngx_pfree(c->pool, b->start) == NGX_OK) {

        /*
         * the special note for ngx_http_keepalive_handler() that
         * c->buffer's memory was freed
         */

        b->pos = NULL;

    } else {
        b->pos = b->start;
        b->last = b->start;
    }

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0, "hc free: %p",
                   hc->free);

    if (hc->free) {
        for (cl = hc->free; cl; /* void */) {
            ln = cl;
            cl = cl->next;
            ngx_pfree(c->pool, ln->buf->start);
            ngx_free_chain(c->pool, ln);
        }

        hc->free = NULL;
    }

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0, "hc busy: %p %i",
                   hc->busy, hc->nbusy);

    if (hc->busy) {
        for (cl = hc->busy; cl; /* void */) {
            ln = cl;
            cl = cl->next;
            ngx_pfree(c->pool, ln->buf->start);
            ngx_free_chain(c->pool, ln);
        }

        hc->busy = NULL;
        hc->nbusy = 0;
    }

#if (NGX_HTTP_SSL)
    if (c->ssl) {
        ngx_ssl_free_buffer(c);
    }
#endif

    rev->handler = ngx_http_keepalive_handler;

    if (wev->active && (ngx_event_flags & NGX_USE_LEVEL_EVENT)) {
        if (ngx_del_event(wev, NGX_WRITE_EVENT, 0) != NGX_OK) {
            ngx_http_close_connection(c);
            return;
        }
    }

    c->log->action = "keepalive";

    if (c->tcp_nopush == NGX_TCP_NOPUSH_SET) {
        if (ngx_tcp_push(c->fd) == -1) {
            ngx_connection_error(c, ngx_socket_errno, ngx_tcp_push_n " failed");
            ngx_http_close_connection(c);
            return;
        }

        c->tcp_nopush = NGX_TCP_NOPUSH_UNSET;
        tcp_nodelay = ngx_tcp_nodelay_and_tcp_nopush ? 1 : 0;

    } else {
        tcp_nodelay = 1;
    }

    if (tcp_nodelay && clcf->tcp_nodelay && ngx_tcp_nodelay(c) != NGX_OK) {
        ngx_http_close_connection(c);
        return;
    }

#if 0
    /* if ngx_http_request_t was freed then we need some other place */
    r->http_state = NGX_HTTP_KEEPALIVE_STATE;
#endif

    c->idle = 1;
    ngx_reusable_connection(c, 1);

    ngx_add_timer(rev, clcf->keepalive_timeout);

    if (rev->ready) {
        ngx_post_event(rev, &ngx_posted_events);
    }
}
```
