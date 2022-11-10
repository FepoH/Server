# 高性能服务器

## 日志模块

**注:** 本项目中:log分成了多个编译单元,可以直接整合成一个.h和.cpp,纯粹为了试一下分文件;
```
日志输出内容含有以下内容:
格式定义:"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
T为Tab键
依次为:日期,线程号,线程名,协程id,日志级别,日志所有者,文件路径,行号,日志内容

输入内容:2022-11-08 18:26:27     111540  iom     fepoh_thr_0       [DEBUG] [root]  /home/fepoh/workspace/fepoh_server/tests/test_iomanager.cpp:25   test
	
// 可变参数处理
// vasprintf是linux的,windows稍微不同
void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

```

## 配置模块
```
采用json,


```


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