# 高性能服务器

## 日志模块

```
采用log4j

主要类:
	LogLevel,LogFormatter,LogAppender,LogEvent,Logger,LogWrap(日志包装器)
	
日志输出内容含有以下内容:
	"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
	文件名,行号,程序启动开始到现在的毫秒数,线程ID,协程ID,时间,线程名字,日志内容字符流,日志级别
	
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