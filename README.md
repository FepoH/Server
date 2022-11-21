<!--
 * @Author: fepo_h
 * @Date: 2022-11-19 17:32:26
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 22:38:16
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
+ 封装Socket读写操作

## Http模块
+ 封装http请求,http响应,http_parser


## json C++

### 语法规则:
+ 数据存储在`名称/值`对中
+ 数据由`逗号,`分隔
+ 使用`\`来转义字符
+ 使用`{}`保存对象
+ 中括号`[]`保存数组,数组可以包含多个对象

## JSON的两种结构:
**对象:**大括号`{}`保存的对象是一个无序的名称/值对集合。一个对象以左括号`{ 开始, 右括号 }`结束，每个"键"后跟一个冒号`:`名称/值对使用逗号`,`分隔。名称是string



**数组:**中括号`[]`保存的数组是值(value)的有序集合。一个数组以左中括号`[开始， 右中括号 ]`结束，值之间使用逗号,分隔。值（value）可以是双引号括起来的字符串（string）、数值(number)、true、false、 null、对象（object）或者数组（array），它们是可以嵌套。


**名称/值对:**
```
"name" : "周乐莎"
```

**JSON值:**
```
数字(整数或浮点数),字符串(在双引号中),逻辑值(true和false),数组(在中括号中),对象(在大括号中),null
```

```
{
    "sites": [
        { "name":"菜鸟教程" , "url":"www.runoob.com" }, 
        { "name":"google" , "url":"www.google.com" }, 
        { "name":"微博" , "url":"www.weibo.com" }
    ]
}
```


```
yaml问题:
在头文件中调用没有问题,但是在cpp文件中调用却出现以下错误
libfepoh_server.so：对‘YAML::Load(std::string const&)’未定义的引用


```