<!--
 * @Author: fepo_h
 * @Date: 2022-11-19 17:32:26
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 20:17:11
 * @FilePath: /fepoh/workspace/fepoh_server/README.md
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
-->
# 高性能服务器

## 日志模块

**注:** 本项目中:log分成了多个编译单元,可以直接整合成一个.h和.cpp,纯粹为了试一下分文件;
### 模块功能:
+ 流式输出,格式化输出
+ 支持定向输出:标准输出和文件输出,通过日志级别可多日志分离输出
+ 支持日志格式自定义,输出时间,线程id,线程名称,行号,函数名称...内容
+ 支持读取配置文件获取日志相关配置"log.json"

### 使用
```C++
//获取日志器
static Logger::ptr logger = FEPOH_LOG_NAME("system");
//流式输出:
FEPOH_LOG_DEBUG(logger) << "I love you three thousand,Miss Zhou";
//格式化输出:
FEPOH_LOG_DEBUG1(logger,"%s%d,%s","I love you ",3000,"Miss Zhou");
//读取配置文件
Config::LoadFromJson("log.json");
```

## 配置模块
### 模块功能
+ 配置模块基于Json,Json库为nlohmann/json,可通过json文件加载配置
+ 适配stl容器:vector,list,set,u_set,map,u_map,可递归解析
+ 支持自定义类型(需实现自定义结构Json<--->std::string)
+ 已实现log自定义类型解析

### 模型
```
//配置模块模型

Json文件->key------>        ==     <------key<-Config
          |                                |
          |        -----tostring-------    |
          v        |                  |    v
      std::string---      Json        --- type
                   |                  |
                   -----fromstring-----

//自定义结构解析模型

            ---->to_json()-------       ----->Json::dump()-----
            |                   |       |                     |
self_struct--                   --Json --                     --std::string
            |                   |       |                     |
            ----from_json()<-----       -----Json::parser()<---

//定义LogAppenderDefine和LogDefine的from_json和to_json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogAppenderDefine, type, level, format,filepath)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogDefine, name, level, appenders)
```
### 使用
```C++
//定义变量
static ConfigVar<uint64_t>::ptr g_http_buffer_size = Config::Lookup<uint64_t>(10 * 1024,"http.buf_size","buffer size");
//获取变量
uint64_t s_buffer_size = g_buffer_size->getValue();
```

## 线程模块
### 模块功能
+ 信号量封装
+ 锁封装(基于pthread的锁):实现互斥锁和读写锁封装,实现自动加锁,解锁
+ pthread线程封装


## 序列化,反序列化模块
### 模块功能
+ 支持文件<--->数据
+ 支持存储定长数据和不定长数据
+ 支持压缩数据(ZigZag算法)
+ 可自适应机器大小端

## 协程调度器模块**
### 协程模型
```C++
//若不适用IOManager,自己创建的协程都是非use_caller,但本人没有实现单独api调用
//非use_caller:
IOManager::t_root_fiber = Fiber::t_main_fiber

             ------->swapIn()--------
             |                      |
t_main_fiber--                      ---other_fiber
             |                      |
             -------swapOut()<-------
             |                      |
             -----swapOutHold()<-----
//use_caller
IOManager::t_root_fiber != Fiber::t_main_fiber
//执行call后,t_root_fiber处于循环中,此时main_fiber暂停,t_root_fiber充当非use_caller中的main_fiber的角色
//back后,t_main_fiber继续执行主流程

             ----->call()------              ----->swapIn()------
             |                |              |                  |
t_main_fiber--                --t_root_fiber--                  ---other_fiber
             |                |              |                  |
             -----back()<------              -----swapOut()<-----
             |                |              |                  |
             ---backHold()<----              ---swapOutHold()<---

```
### 调度器功能
+ 创建线程池
+ 创建一个比例为:`调度器:协程:协程=1:N:M`的协程调度器,调度协程任务
+ 支持任务在多线程中切换,有效防止线程资源分配不均匀问题

### 使用
```C++
void test(){
    //...
}

void test(){
    /**
     * @param1 调度器名称
     * @param2 线程数
     * @param3 use_caller
     */    
    IOManager iom("name",3,true);
    iom.schedule(test);
    iom.start();
    iom.stop();
}
```
## IO协程调度
### 功能
+ 基于epoll的et边缘触发,在线程无事可做时陷入epoll_wait
+ 继承协程调度器,继承定时器(毫秒级别精度),可添加定时器和条件定时器
+ 可添加,删除,取消socket的读写事件

### 定时器(毫秒级精度)
```C++
void test(){
    //....
}

void test1(){
    TimerManager tm;
    tm.addTimer(1000,test,true);
    //cond为weak_ptr
    tm.addConditionTimer(1000,test,cond,false);
    std::vector<std::function<void()> > cbs;
    tm->listExpiredCb(cbs);
    for(auto& cb:cbs){
        cb();
    }
}
```
## Hook模块
### 功能
+ hook系统底层和socket相关的API，socket io相关的API，以及sleep系列的API。
+ hook的开启控制是线程粒度的,可以自由选择。通过hook模块，可以使一些不具异步功能的API，展现出异步的性能。

## Socket模块
### 功能
+ 封装unix,ipv4,ipv6地址,定义常用API
+ 封装socket,集成相关操作,读写数据,bind,accept,connect...
+ 提供域名解析功能

## Stream模块
### 功能
+ 封装Socket读写操作,实现继承类sock_stream操作对socket的读写

## Http模块
+ 封装http请求,http响应,http_parser,uri
+ 封装http服务端和http请求端相关操作
+ 封装连接池

## 其他模块
+ util,marco,nocopyable.endian...

## 性能测试
### 采用ab进行压测
```
//压测数据
//单线程
Server Software:        epoh/1.0.0
Server Hostname:        192.168.88.129
Server Port:            8020

Document Path:          /
Document Length:        130 bytes

Concurrency Level:      200
Time taken for tests:   63.946 seconds
Complete requests:      1000000
Failed requests:        0
Write errors:           0
Non-2xx responses:      1000000
Total transferred:      238000000 bytes
HTML transferred:       130000000 bytes
Requests per second:    15638.31 [#/sec] (mean)/14680.81 [#/sec] (mean)/14849.82 [#/sec] (mean)
Time per request:       12.789 [ms] (mean)
Time per request:       0.064 [ms] (mean, across all concurrent requests)
Transfer rate:          3634.69 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    4  60.0      1    3031
Processing:     2    8   3.3      8     409
Waiting:        1    8   3.2      8     408
Total:          5   13  60.2      9    3039

Percentage of the requests served within a certain time (ms)
  50%      9
  66%      9
  75%     10
  80%     10
  90%     12
  95%     13
  98%     14
  99%     16
 100%   3039 (longest request)

//多线程含有一个偶发bug,实在找不到...,暂未测测试
```