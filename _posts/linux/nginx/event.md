---
title: event事件模块
date: 2020-06-09 16:22
categories: 
tags: 
---

事件驱动是nginx高性能的基石也是核心, 正是基于事件驱动构造永不阻塞的线程运行机制, 减少了线程的上下文切换带来的损耗, 从而造就了nginx的高性能
除了事件部分, connection的创建销毁也是event模块关注的

## 配置解析
### ngx_events_module
首先是event的**NGX_CORE_MODULE**类型模块
```c
ngx_module_t  ngx_events_module = {
    NGX_MODULE_V1,
    &ngx_events_module_ctx,                /* module context */
    ngx_events_commands,                   /* module directives */
    NGX_CORE_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_core_module_t  ngx_events_module_ctx = {
    ngx_string("events"),
    NULL,

    // 1.15.2之前只检查是否创建了配置结构体，无其他操作
    // 因为event模块只有一个events指令
    // 1.15.2之后增加新的代码
    // 主要是对套接字进行了检查
    ngx_event_init_conf
};

// 1.15.2之前只检查是否创建了配置结构体，无其他操作
// 因为event模块只有一个events指令
// 1.15.2之后增加新的代码
static char *
ngx_event_init_conf(ngx_cycle_t *cycle, void *conf)
{
    // 1.15.2新增部分检查代码
#if (NGX_HAVE_REUSEPORT)
    ngx_uint_t        i;
    ngx_listening_t  *ls;
#endif

    // 要求必须有events{}配置块
    // 1.15.2之前只有这一段
    if (ngx_get_conf(cycle->conf_ctx, ngx_events_module) == NULL) {
        ngx_log_error(NGX_LOG_EMERG, cycle->log, 0,
                      "no \"events\" section in configuration");
        return NGX_CONF_ERROR;
    }

    // 检查连接数，需要大于监听端口数量
    if (cycle->connection_n < cycle->listening.nelts + 1) {

        /*
         * there should be at least one connection for each listening
         * socket, plus an additional connection for channel
         */

        ngx_log_error(NGX_LOG_EMERG, cycle->log, 0,
                      "%ui worker_connections are not enough "
                      "for %ui listening sockets",
                      cycle->connection_n, cycle->listening.nelts);

        return NGX_CONF_ERROR;
    }

#if (NGX_HAVE_REUSEPORT)

    ls = cycle->listening.elts;
    for (i = 0; i < cycle->listening.nelts; i++) {

        if (!ls[i].reuseport || ls[i].worker != 0) {
            continue;
        }

        // reuseport专用的函数，1.8.x没有
        // 拷贝了worker数量个的监听结构体, in ngx_connection.c
        // 从ngx_stream_optimize_servers等函数处转移过来
        if (ngx_clone_listening(cycle, &ls[i]) != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        /* cloning may change cycle->listening.elts */

        ls = cycle->listening.elts;
    }

#endif

    return NGX_CONF_OK;
}

// 1.10新函数，专为reuseport使用
// 拷贝了worker数量个的监听结构体
// 在ngx_stream_optimize_servers等函数创建端口时调用
ngx_int_t
ngx_clone_listening(ngx_cycle_t *cycle, ngx_listening_t *ls)
{
// configure脚本可以检测系统是否支持reuseport
// 使用宏来控制条件编译
#if (NGX_HAVE_REUSEPORT)

    ngx_int_t         n;
    ngx_core_conf_t  *ccf;
    ngx_listening_t   ols;

    // 监听指令需要配置了reuseport
    if (!ls->reuseport || ls->worker != 0) {
        return NGX_OK;
    }

    // ls在之前已经正确设置了若干参数
    // 例如type/handler/backlog等等
    ols = *ls;

    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx, ngx_core_module);

    // ccf->worker_processes是nginx的worker进程数
    // 拷贝了worker数量个的监听结构体
    //
    // 注意从1开始，这样拷贝后总数就是worker数量个
    // 最开始的一个就是被克隆的ls监听结构体
    for (n = 1; n < ccf->worker_processes; n++) {

        /* create a socket for each worker process */

        ls = ngx_array_push(&cycle->listening);
        if (ls == NULL) {
            return NGX_ERROR;
        }

        // 完全拷贝结构体
        *ls = ols;

        // 设置worker的序号
        // 被克隆的对象的worker是0
        //
        // worker的使用是在ngx_event.c:ngx_event_process_init
        // 只有worker id是本worker的listen才会enable
        ls->worker = n;
    }

#endif

    return NGX_OK;
}
```
#### 命令
##### events
event模块只有一个**events**命令, 用来放其他事件模块的命令

### ngx_event_core_module_t
这里出现了第一个**ngx_event_module_t**
```c
// ngx_event_module_t
// 事件模块的函数指针表
// 核心是actions，即事件处理函数
typedef struct {
    // 事件模块的名字，如epoll/select/kqueue
    ngx_str_t              *name;

    // 事件模块的配置相关函数比较简单
    void                 *(*create_conf)(ngx_cycle_t *cycle);
    char                 *(*init_conf)(ngx_cycle_t *cycle, void *conf);

    // 事件模块访问接口，是一个函数表
    ngx_event_actions_t     actions;
} ngx_event_module_t;

// ngx_event_actions_t
// 全局的事件模块访问接口，是一个函数表
// 由epoll/kqueue/select等模块实现
// epoll的实现在modules/ngx_epoll_module.c
typedef struct {
    // 添加事件,事件发生时epoll调用可以获取
    // epoll添加事件
    // 检查事件关联的连接对象，决定是新添加还是修改
    // 避免误删除了读写事件的关注
    ngx_int_t  (*add)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    // 删除事件,epoll不再关注该事件
    // epoll删除事件
    // 检查事件关联的连接对象，决定是完全删除还是修改
    // 避免误删除了读写事件的关注
    ngx_int_t  (*del)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    // 同add
    ngx_int_t  (*enable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    // 同del
    ngx_int_t  (*disable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    // 添加一个连接，也就是读写事件都添加
    // epoll关注连接的读写事件
    // 添加事件成功，读写事件都是活跃的，即已经使用
    ngx_int_t  (*add_conn)(ngx_connection_t *c);

    // 删除一个连接，该连接的读写事件都不再关注
    // epoll删除连接的读写事件
    // 删除事件成功，读写事件都不活跃
    ngx_int_t  (*del_conn)(ngx_connection_t *c, ngx_uint_t flags);

    // 目前仅多线程使用，通知
    // 调用系统函数eventfd，创建一个可以用于通知的描述符，用于实现notify
    // 模仿此用法也可以实现自己的通知机制
    ngx_int_t  (*notify)(ngx_event_handler_pt handler);

    // 事件模型的核心功能，处理发生的事件
    //
    // epoll模块核心功能，调用epoll_wait处理发生的事件
    // 使用event_list和nevents获取内核返回的事件
    // timer是无事件发生时最多等待的时间，即超时时间
    // 函数可以分为两部分，一是用epoll获得事件，二是处理事件，加入延后队列
    // 在ngx_process_events_and_timers里被调用
    ngx_int_t  (*process_events)(ngx_cycle_t *cycle, ngx_msec_t timer,
                                 ngx_uint_t flags);

    // 初始化事件模块
    // 调用epoll_create初始化epoll机制
    // 参数size=cycle->connection_n / 2，但并无实际意义
    // 设置全局变量，操作系统提供的底层数据收发接口
    // 初始化全局的事件模块访问接口，指向epoll的函数
    // 默认使用et模式，边缘触发，高速
    ngx_int_t  (*init)(ngx_cycle_t *cycle, ngx_msec_t timer);

    // 事件模块结束时的收尾工作
    // epoll模块结束工作，关闭epoll句柄和通知句柄，释放内存
    void       (*done)(ngx_cycle_t *cycle);
} ngx_event_actions_t;

// 全局的事件模块访问接口，是一个函数表
// 定义了若干宏简化对它的操作
// 常用的有ngx_add_event/ngx_del_event
// 在epoll模块的ngx_epoll_init里设置，指向epoll的函数
// ngx_event_actions = ngx_epoll_module_ctx.actions;
extern ngx_event_actions_t   ngx_event_actions;
// 访问的时候就可以直接actions.add 访问具体的事件模块定义的方法
```

```c
// 但它不实现具体的事件模型，所以actions函数表全是空指针
static ngx_event_module_t  ngx_event_core_module_ctx = {
    // event_core模块的名字："event_core"
    &event_core_name,

    // 创建event_core模块的配置结构体，成员初始化为unset
    ngx_event_core_create_conf,            /* create configuration */

    // 所有模块配置解析完毕后，对配置进行初始化
    // 如果有的指令没有写，就要给正确的默认值
    // 模块默认使用epoll
    // 默认不接受多个请求，也就是一次只accept一个连接
    // 1.11.3之前默认使用负载均衡锁，之后默认关闭
    ngx_event_core_init_conf,              /* init configuration */

    // 不实现具体的事件模型，所以actions函数表全是空指针
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

// 创建event_core模块的配置结构体，成员初始化为unset
// event_core的配置项好少
static void *
ngx_event_core_create_conf(ngx_cycle_t *cycle)
{
    ngx_event_conf_t  *ecf;

    ecf = ngx_palloc(cycle->pool, sizeof(ngx_event_conf_t));
    if (ecf == NULL) {
        return NULL;
    }

    ecf->connections = NGX_CONF_UNSET_UINT;
    ecf->use = NGX_CONF_UNSET_UINT;
    ecf->multi_accept = NGX_CONF_UNSET;
    ecf->accept_mutex = NGX_CONF_UNSET;
    ecf->accept_mutex_delay = NGX_CONF_UNSET_MSEC;
    ecf->name = (void *) NGX_CONF_UNSET;

#if (NGX_DEBUG)

    if (ngx_array_init(&ecf->debug_connection, cycle->pool, 4,
                       sizeof(ngx_cidr_t)) == NGX_ERROR)
    {
        return NULL;
    }

#endif

    return ecf;
}

// 所有模块配置解析完毕后，对配置进行初始化
// 如果有的指令没有写，就要给正确的默认值
// 模块默认使用epoll
// 默认不接受多个请求，也就是一次只accept一个连接
// 1.11.3之前默认使用负载均衡锁，之后默认关闭
static char *
ngx_event_core_init_conf(ngx_cycle_t *cycle, void *conf)
{
    ngx_event_conf_t  *ecf = conf;

#if (NGX_HAVE_EPOLL) && !(NGX_TEST_BUILD_EPOLL)
    int                  fd;
#endif

    // rtsig在nginx 1.9.x已经删除

    ngx_int_t            i;
    ngx_module_t        *module;
    ngx_event_module_t  *event_module;

    module = NULL;

// 测试epoll是否可用
#if (NGX_HAVE_EPOLL) && !(NGX_TEST_BUILD_EPOLL)

    fd = epoll_create(100);

    // epoll调用可用，那么模块默认使用epoll
    if (fd != -1) {
        (void) close(fd);
        // epoll调用可用，那么模块默认使用epoll
        module = &ngx_epoll_module;

    } else if (ngx_errno != NGX_ENOSYS) {
        // epoll调用可用，那么模块默认使用epoll
        module = &ngx_epoll_module;
    }

#endif

    // rtsig在nginx 1.9.x已经删除

#if (NGX_HAVE_DEVPOLL) && !(NGX_TEST_BUILD_DEVPOLL)

    module = &ngx_devpoll_module;

#endif

#if (NGX_HAVE_KQUEUE)

    module = &ngx_kqueue_module;

#endif

    // 如果epoll不可用，那么默认使用select
#if (NGX_HAVE_SELECT)

    if (module == NULL) {
        module = &ngx_select_module;
    }

#endif

    // 还没有决定默认的事件模型
    if (module == NULL) {
        // 遍历所有的事件模块
        for (i = 0; cycle->modules[i]; i++) {

            if (cycle->modules[i]->type != NGX_EVENT_MODULE) {
                continue;
            }

            event_module = cycle->modules[i]->ctx;

            // 跳过event_core模块
            if (ngx_strcmp(event_module->name->data, event_core_name.data) == 0)
            {
                continue;
            }

            // 使用数组里的第一个事件模块
            module = cycle->modules[i];
            break;
        }
    }

    // 最后还没有决定默认的事件模型，出错
    if (module == NULL) {
        ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "no events module found");
        return NGX_CONF_ERROR;
    }

    // nginx每个进程可使用的连接数量，即cycle里的连接池大小
    ngx_conf_init_uint_value(ecf->connections, DEFAULT_CONNECTIONS);

    // 如果没有使用worker_connections指令，在这里设置
    cycle->connection_n = ecf->connections;

    // 决定使用的事件模型,之前的module只作为默认值，如果已经使用了use则无效
    ngx_conf_init_uint_value(ecf->use, module->ctx_index);

    // 初始化使用的事件模块的名字
    event_module = module->ctx;
    ngx_conf_init_ptr_value(ecf->name, event_module->name->data);

    // 默认不接受多个请求，也就是一次只accept一个连接
    ngx_conf_init_value(ecf->multi_accept, 0);

    // 1.11.3之前默认使用负载均衡锁，之后默认关闭
    ngx_conf_init_value(ecf->accept_mutex, 0);

    // 默认负载均衡锁的等待时间是500毫秒
    ngx_conf_init_msec_value(ecf->accept_mutex_delay, 500);

    return NGX_CONF_OK;
}

```
```c
ngx_module_t  ngx_event_core_module = {
    NGX_MODULE_V1,
    &ngx_event_core_module_ctx,            /* module context */
    ngx_event_core_commands,               /* module directives */
    NGX_EVENT_MODULE,                      /* module type */
    NULL,                                  /* init master */

    // 在ngx_init_cycle里调用，fork子进程之前
    // 创建共享内存，存放负载均衡锁和统计用的原子变量
    ngx_event_module_init,                 /* init module */

    // 初始化cycle里的连接和事件数组
    // fork之后，worker进程初始化时调用，即每个worker里都会执行
    // 初始化两个延后处理的事件队列,初始化定时器红黑树
    // 发送定时信号，更新时间用
    // 初始化cycle里的连接和事件数组
    // 设置接受连接的回调函数为ngx_event_accept，可以接受连接
    ngx_event_process_init,                /* init process */

    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

// 在ngx_init_cycle里调用，fork子进程之前
// 创建共享内存，存放负载均衡锁和统计用的原子变量
static ngx_int_t
ngx_event_module_init(ngx_cycle_t *cycle)
{
    void              ***cf;
    u_char              *shared;
    size_t               size, cl;
    ngx_shm_t            shm;
    ngx_time_t          *tp;
    ngx_core_conf_t     *ccf;
    ngx_event_conf_t    *ecf;

    // events模块的配置结构体
    // 实际上是一个存储void*指针的数组
    cf = ngx_get_conf(cycle->conf_ctx, ngx_events_module);

    // event_core模块的配置结构体
    // 从数组cf里按序号查找
    ecf = (*cf)[ngx_event_core_module.ctx_index];

    // 上面的两行代码相当于:
    // ngx_event_get_conf(cycle->conf_ctx, ngx_event_core_module)

    if (!ngx_test_config && ngx_process <= NGX_PROCESS_MASTER) {
        ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0,
                      "using the \"%s\" event method", ecf->name);
    }

    // core模块的配置结构体
    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx, ngx_core_module);

    // 获取核心配置的时间精度，用在epoll里更新缓存时间
    ngx_timer_resolution = ccf->timer_resolution;

    // unix专用代码, 可打开的最多文件描述符
#if !(NGX_WIN32)
    {
    ngx_int_t      limit;
    struct rlimit  rlmt;

    // 系统调用getrlimit，Linux内核对进程的限制
    // RLIMIT_NOFILE,进程可打开的最大文件描述符数量，超出将产生EMFILE错误
    if (getrlimit(RLIMIT_NOFILE, &rlmt) == -1) {

        // 系统调用失败则记录alert级别日志
        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                      "getrlimit(RLIMIT_NOFILE) failed, ignored");

    } else {
        // 成功获取内核参数
        //
        // rlmt.rlim_cur是系统的软限制
        // event里配置的连接数不能超过系统内核限制
        // 或者是配置的rlimit_nofile限制
        if (ecf->connections > (ngx_uint_t) rlmt.rlim_cur
            && (ccf->rlimit_nofile == NGX_CONF_UNSET
                || ecf->connections > (ngx_uint_t) ccf->rlimit_nofile))
        {
            // 如果超过了报警告级别日志
            // limit就是上限
            limit = (ccf->rlimit_nofile == NGX_CONF_UNSET) ?
                         (ngx_int_t) rlmt.rlim_cur : ccf->rlimit_nofile;

            ngx_log_error(NGX_LOG_WARN, cycle->log, 0,
                          "%ui worker_connections exceed "
                          "open file resource limit: %i",
                          ecf->connections, limit);
        }
    }
    }
#endif /* !(NGX_WIN32) */


    // 如果非master/worker进程，即只启动一个进程，那么就没必要使用负载均衡锁
    if (ccf->master == 0) {
        return NGX_OK;
    }

    // 已经有了负载均衡锁，已经初始化过了，就没必要再做操作
    if (ngx_accept_mutex_ptr) {
        return NGX_OK;
    }


    /* cl should be equal to or greater than cache line size */

    // cl是一个基本长度，可以容纳原子变量
    // 对齐到cache line，操作更快
    cl = 128;

    // 最基本的三个：负载均衡锁，连接计数器,
    size = cl            /* ngx_accept_mutex */
           + cl          /* ngx_connection_counter */
           + cl;         /* ngx_temp_number */

    // 其他统计用的原子变量
#if (NGX_STAT_STUB)

    size += cl           /* ngx_stat_accepted */
           + cl          /* ngx_stat_handled */
           + cl          /* ngx_stat_requests */
           + cl          /* ngx_stat_active */
           + cl          /* ngx_stat_reading */
           + cl          /* ngx_stat_writing */
           + cl;         /* ngx_stat_waiting */

#endif

    // 创建共享内存，存放负载均衡锁和统计用的原子变量
    // 因为内存很小，而且仅用做统计，比较简单
    // 所以不用slab管理
    shm.size = size;
    ngx_str_set(&shm.name, "nginx_shared_zone");
    shm.log = cycle->log;

    // 分配一块共享内存
    if (ngx_shm_alloc(&shm) != NGX_OK) {
        return NGX_ERROR;
    }

    // shared是共享内存的地址指针
    shared = shm.addr;

    // 第一个就是负载均衡锁
    ngx_accept_mutex_ptr = (ngx_atomic_t *) shared;

    // spin是-1则不使用信号量
    // 只会自旋，不会导致进程睡眠等待
    // 这样避免抢accept锁时的性能降低
    ngx_accept_mutex.spin = (ngx_uint_t) -1;

    // 初始化互斥锁
    // spin是-1则不使用信号量
    // 只会自旋，不会导致进程睡眠等待
    if (ngx_shmtx_create(&ngx_accept_mutex, (ngx_shmtx_sh_t *) shared,
                         cycle->lock_file.data)
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    // 连接计数器
    ngx_connection_counter = (ngx_atomic_t *) (shared + 1 * cl);

    // 计数器置1
    (void) ngx_atomic_cmp_set(ngx_connection_counter, 0, 1);

    ngx_log_debug2(NGX_LOG_DEBUG_EVENT, cycle->log, 0,
                   "counter: %p, %uA",
                   ngx_connection_counter, *ngx_connection_counter);

    // 临时文件用
    ngx_temp_number = (ngx_atomic_t *) (shared + 2 * cl);

    tp = ngx_timeofday();

    // 随机数
    // 每个进程不同
    ngx_random_number = (tp->msec << 16) + ngx_pid;

#if (NGX_STAT_STUB)

    ngx_stat_accepted = (ngx_atomic_t *) (shared + 3 * cl);
    ngx_stat_handled = (ngx_atomic_t *) (shared + 4 * cl);
    ngx_stat_requests = (ngx_atomic_t *) (shared + 5 * cl);
    ngx_stat_active = (ngx_atomic_t *) (shared + 6 * cl);
    ngx_stat_reading = (ngx_atomic_t *) (shared + 7 * cl);
    ngx_stat_writing = (ngx_atomic_t *) (shared + 8 * cl);
    ngx_stat_waiting = (ngx_atomic_t *) (shared + 9 * cl);

#endif

    return NGX_OK;
}


#if !(NGX_WIN32)

// sigalarm信号的处理函数，只设置ngx_event_timer_alarm变量
// 在epoll的ngx_epoll_process_events里检查，更新时间的标志
// 信号处理函数应该尽量简单，避免阻塞进程
static void
ngx_timer_signal_handler(int signo)
{
    ngx_event_timer_alarm = 1;

#if 1
    ngx_log_debug0(NGX_LOG_DEBUG_EVENT, ngx_cycle->log, 0, "timer signal");
#endif
}

#endif


// fork之后，worker进程初始化时调用，即每个worker里都会执行
// 初始化两个延后处理的事件队列,初始化定时器红黑树
// 发送定时信号，更新时间用
// 初始化cycle里的连接和事件数组
// 设置接受连接的回调函数为ngx_event_accept，可以接受连接
static ngx_int_t
ngx_event_process_init(ngx_cycle_t *cycle)
{
    ngx_uint_t           m, i;
    ngx_event_t         *rev, *wev;
    ngx_listening_t     *ls;
    ngx_connection_t    *c, *next, *old;
    ngx_core_conf_t     *ccf;
    ngx_event_conf_t    *ecf;
    ngx_event_module_t  *module;

    // core模块的配置结构体
    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx, ngx_core_module);

    // event_core模块的配置结构体
    ecf = ngx_event_get_conf(cycle->conf_ctx, ngx_event_core_module);

    // 使用master/worker多进程，使用负载均衡
    if (ccf->master && ccf->worker_processes > 1 && ecf->accept_mutex) {

        // 设置全局变量
        // 使用负载均衡，刚开始未持有锁，设置抢锁的等待时间
        ngx_use_accept_mutex = 1;
        ngx_accept_mutex_held = 0;
        ngx_accept_mutex_delay = ecf->accept_mutex_delay;

    } else {
        // 单进程、未明确指定负载均衡，就不使用负载均衡
        ngx_use_accept_mutex = 0;
    }

#if (NGX_WIN32)

    /*
     * disable accept mutex on win32 as it may cause deadlock if
     * grabbed by a process which can't accept connections
     */

    ngx_use_accept_mutex = 0;

#endif

    // 初始化两个延后处理的事件队列
    ngx_queue_init(&ngx_posted_accept_events);
    ngx_queue_init(&ngx_posted_next_events);
    ngx_queue_init(&ngx_posted_events);

    // 初始化定时器红黑树
    if (ngx_event_timer_init(cycle->log) == NGX_ERROR) {
        return NGX_ERROR;
    }

    // 遍历事件模块，但只执行实际使用的事件模块对应初始化函数
    for (m = 0; cycle->modules[m]; m++) {
        if (cycle->modules[m]->type != NGX_EVENT_MODULE) {
            continue;
        }

        // 找到use指令使用的事件模型，或者是默认事件模型
        if (cycle->modules[m]->ctx_index != ecf->use) {
            continue;
        }

        // 找到事件模块

        module = cycle->modules[m]->ctx;

        // 调用事件模块的事件初始化函数
        //
        // 调用epoll_create初始化epoll机制
        // 参数size=cycle->connection_n / 2，但并无实际意义
        // 设置全局变量，操作系统提供的底层数据收发接口
        // 初始化全局的事件模块访问接口，指向epoll的函数
        // 默认使用et模式，边缘触发，高速
        if (module->actions.init(cycle, ngx_timer_resolution) != NGX_OK) {
            /* fatal */
            exit(2);
        }

        // 找到一个事件模块即退出循环
        // 也就是说只能使用一种事件模型
        break;
    }

// unix代码, 发送定时信号，更新时间用
#if !(NGX_WIN32)

    // NGX_USE_TIMER_EVENT标志量只有eventport/kqueue,epoll无此标志位
    // ngx_timer_resolution = ccf->timer_resolution;默认值是0
    // 所以只有使用了timer_resolution指令才会发信号
    if (ngx_timer_resolution && !(ngx_event_flags & NGX_USE_TIMER_EVENT)) {
        struct sigaction  sa;
        struct itimerval  itv;

        // 设置信号掩码，sigalarm
        ngx_memzero(&sa, sizeof(struct sigaction));
        sa.sa_handler = ngx_timer_signal_handler;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                          "sigaction(SIGALRM) failed");
            return NGX_ERROR;
        }

        // 设置信号发送的时间间隔，也就是nginx的时间精度
        // 收到信号会设置设置ngx_event_timer_alarm变量
        // 在epoll的ngx_epoll_process_events里检查，更新时间的标志
        itv.it_interval.tv_sec = ngx_timer_resolution / 1000;
        itv.it_interval.tv_usec = (ngx_timer_resolution % 1000) * 1000;
        itv.it_value.tv_sec = ngx_timer_resolution / 1000;
        itv.it_value.tv_usec = (ngx_timer_resolution % 1000 ) * 1000;

        if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                          "setitimer() failed");
        }
    }

    // poll, /dev/poll进入这个分支处理
    if (ngx_event_flags & NGX_USE_FD_EVENT) {
        struct rlimit  rlmt;

        if (getrlimit(RLIMIT_NOFILE, &rlmt) == -1) {
            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                          "getrlimit(RLIMIT_NOFILE) failed");
            return NGX_ERROR;
        }

        cycle->files_n = (ngx_uint_t) rlmt.rlim_cur;

        cycle->files = ngx_calloc(sizeof(ngx_connection_t *) * cycle->files_n,
                                  cycle->log);
        if (cycle->files == NULL) {
            return NGX_ERROR;
        }
    }

#else

    if (ngx_timer_resolution && !(ngx_event_flags & NGX_USE_TIMER_EVENT)) {
        ngx_log_error(NGX_LOG_WARN, cycle->log, 0,
                      "the \"timer_resolution\" directive is not supported "
                      "with the configured event method, ignored");
        ngx_timer_resolution = 0;
    }

#endif

    // 创建连接池数组，大小是cycle->connection_n
    // 直接使用malloc分配内存，没有使用内存池
    cycle->connections =
        ngx_alloc(sizeof(ngx_connection_t) * cycle->connection_n, cycle->log);
    if (cycle->connections == NULL) {
        return NGX_ERROR;
    }

    c = cycle->connections;

    // 创建读事件池数组，大小是cycle->connection_n
    cycle->read_events = ngx_alloc(sizeof(ngx_event_t) * cycle->connection_n,
                                   cycle->log);
    if (cycle->read_events == NULL) {
        return NGX_ERROR;
    }

    // 读事件对象初始化
    rev = cycle->read_events;
    for (i = 0; i < cycle->connection_n; i++) {
        rev[i].closed = 1;
        rev[i].instance = 1;
    }

    // 创建写事件池数组，大小是cycle->connection_n
    cycle->write_events = ngx_alloc(sizeof(ngx_event_t) * cycle->connection_n,
                                    cycle->log);
    if (cycle->write_events == NULL) {
        return NGX_ERROR;
    }

    // 写事件对象初始化
    wev = cycle->write_events;
    for (i = 0; i < cycle->connection_n; i++) {
        wev[i].closed = 1;
    }

    // i是数组的末尾
    i = cycle->connection_n;
    next = NULL;

    // 把连接对象与读写事件关联起来
    // 注意i是数组的末尾，从最后遍历
    do {
        i--;

        // 使用data成员，把连接对象串成链表
        c[i].data = next;

        // 读写事件
        c[i].read = &cycle->read_events[i];
        c[i].write = &cycle->write_events[i];

        // 连接的描述符是-1，表示无效
        c[i].fd = (ngx_socket_t) -1;

        // next指针指向数组的前一个元素
        next = &c[i];
    } while (i);

    // 连接对象已经串成链表，现在设置空闲链表指针
    // 此时next指向连接对象数组的第一个元素
    cycle->free_connections = next;

    // 连接没有使用，全是空闲连接
    cycle->free_connection_n = cycle->connection_n;

    /* for each listening socket */

    // 为每个监听端口分配一个连接对象
    ls = cycle->listening.elts;
    for (i = 0; i < cycle->listening.nelts; i++) {

#if (NGX_HAVE_REUSEPORT)
        // 注意这里
        // 只有worker id是本worker的listen才会enable
        // 也就是说虽然克隆了多个listening，但只有一个会enable
        // 即reuseport的端口只会在某个worker进程监听
        if (ls[i].reuseport && ls[i].worker != ngx_worker) {
            continue;
        }
#endif

        // 获取一个空闲连接
        c = ngx_get_connection(ls[i].fd, cycle->log);

        if (c == NULL) {
            return NGX_ERROR;
        }

        c->type = ls[i].type;
        c->log = &ls[i].log;

        // 连接的listening对象
        // 两者相互连接
        c->listening = &ls[i];
        ls[i].connection = c;

        // 监听端口只关心读事件
        rev = c->read;

        rev->log = c->log;

        // 设置accept标志，接受连接
        rev->accept = 1;

#if (NGX_HAVE_DEFERRED_ACCEPT)
        rev->deferred_accept = ls[i].deferred_accept;
#endif

        if (!(ngx_event_flags & NGX_USE_IOCP_EVENT)) {
            if (ls[i].previous) {

                /*
                 * delete the old accept events that were bound to
                 * the old cycle read events array
                 */

                old = ls[i].previous->connection;

                if (ngx_del_event(old->read, NGX_READ_EVENT, NGX_CLOSE_EVENT)
                    == NGX_ERROR)
                {
                    return NGX_ERROR;
                }

                old->fd = (ngx_socket_t) -1;
            }
        }

#if (NGX_WIN32)

        if (ngx_event_flags & NGX_USE_IOCP_EVENT) {
            ngx_iocp_conf_t  *iocpcf;

            rev->handler = ngx_event_acceptex;

            if (ngx_use_accept_mutex) {
                continue;
            }

            if (ngx_add_event(rev, 0, NGX_IOCP_ACCEPT) == NGX_ERROR) {
                return NGX_ERROR;
            }

            ls[i].log.handler = ngx_acceptex_log_error;

            iocpcf = ngx_event_get_conf(cycle->conf_ctx, ngx_iocp_module);
            if (ngx_event_post_acceptex(&ls[i], iocpcf->post_acceptex)
                == NGX_ERROR)
            {
                return NGX_ERROR;
            }

        } else {
            rev->handler = ngx_event_accept;

            if (ngx_use_accept_mutex) {
                continue;
            }

            if (ngx_add_event(rev, NGX_READ_EVENT, 0) == NGX_ERROR) {
                return NGX_ERROR;
            }
        }

#else

        // 重要！！
        // 设置接受连接的回调函数为ngx_event_accept
        // 监听端口上收到连接请求时的回调函数，即事件handler
        // 从cycle的连接池里获取连接
        // 关键操作 ls->handler(c);调用其他模块的业务handler
        // 1.10使用ngx_event_recvmsg接收udp
        rev->handler = (c->type == SOCK_STREAM) ? ngx_event_accept
                                                : ngx_event_recvmsg;

#if (NGX_HAVE_REUSEPORT)

        // reuseport无视负载均衡，直接开始监听
        if (ls[i].reuseport) {
            if (ngx_add_event(rev, NGX_READ_EVENT, 0) == NGX_ERROR) {
                return NGX_ERROR;
            }

            continue;
        }

#endif

        // 如果使用负载均衡，不向epoll添加事件，只有抢到锁才添加
        if (ngx_use_accept_mutex) {
            continue;
        }

#if (NGX_HAVE_EPOLLEXCLUSIVE)

        if ((ngx_event_flags & NGX_USE_EPOLL_EVENT)
            && ccf->worker_processes > 1)
        {
            // nginx 1.9.x不再使用rtsig

            // 单进程、未明确指定负载均衡，不使用负载均衡
            // 直接加入epoll事件，开始监听，可以接受请求
            // 如果支持EPOLLEXCLUSIVE，使用特殊的标志位
            if (ngx_add_event(rev, NGX_READ_EVENT, NGX_EXCLUSIVE_EVENT)
                == NGX_ERROR)
            {
                return NGX_ERROR;
            }

            // 跳过下面的普通add event
            continue;
        }

#endif

        // 单进程、未明确指定负载均衡，不使用负载均衡
        // 直接加入epoll事件，开始监听，可以接受请求
        if (ngx_add_event(rev, NGX_READ_EVENT, 0) == NGX_ERROR) {
            return NGX_ERROR;
        }

#endif

    } // 为每个监听端口分配一个连接对象循环结束

    return NGX_OK;
}
```

#### 指令
##### worker_connections
单个进程所能创建的最大连接数, 注意不只是代理的连接, 是进程上用到的所有连接

##### use
用哪个事件响应机制

##### multi_accept
决定了一次accept能接收多少个请求, off一次接1个, on一次能接多少接多少

##### accept_mutex
负载均衡锁

##### accept_mutex_delay
负载均衡锁上等待最多多少时间

##### debug_connection
> debug_connection address | CIDR | unix:;
对某些client打印debug日志






