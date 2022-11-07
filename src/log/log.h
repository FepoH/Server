#pragma once

#include "logger.h"
#include "log_manager.h"
#include "util.h"
#include <nlohmann/json.hpp>


/*
    日志的所有类可以集合在一个文件中,笔者有意分开试一下,建议放在一起,这样更加方便
*/

//流式输出
//格式为:FEPOH_LOG_DEBUG(logger) << "I'll think of you every step of the way.";
#define FEPOH_LOG_LEVEL(logger,level)   \
    if(level>=logger->getLevel())        \
        fepoh::LogEventWrap(fepoh::LogEvent::ptr(new fepoh::LogEvent(__FILE__,__LINE__,0,fepoh::GetThreadId(),0,::time(0),fepoh::GetThreadName())),logger,level).getSS()

#define FEPOH_LOG_DEBUG(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::DEBUG)
#define FEPOH_LOG_INFO(logger)  FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::INFO)
#define FEPOH_LOG_WARN(logger)  FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::WARN)
#define FEPOH_LOG_ERROR(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::ERROR)
#define FEPOH_LOG_FATAL(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::FATAL)


#define FEPOH_LOG_FORMAT(logger,level,...)  \
    if(level>=logger->getLevel())           \
        fepoh::LogEventWrap(fepoh::LogEvent::ptr(new fepoh::LogEvent(__FILE__,__LINE__,0,fepoh::GetThreadId(),0,time(0),"fepoh")),logger,level).format(__VA_ARGS__);

//格式化输出
//格式为:FEPOH_LOG_DEBUG1(logger,"i = %d ,str = %s",10,"I love zls three thousand times");
#define FEPOH_LOG_DEBUG1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::DEBUG,__VA_ARGS__)
#define FEPOH_LOG_INFO1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::INFO,__VA_ARGS__)
#define FEPOH_LOG_WARN1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::WARN,__VA_ARGS__)
#define FEPOH_LOG_ERROR1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::ERROR,__VA_ARGS__)
#define FEPOH_LOG_FATAL1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::FATAL,__VA_ARGS__)

#define FEPOH_LOG_NAME(name)    \
    fepoh::LogManager::GetInstance()->getLogger(name)


namespace fepoh{

/*
//
//  日志的配置文件完成配置文件转换,格式详情看下
//

{
  "logs": [
    {
      "name": "root",       //日志器名称
      "level": "debug",     //日志器日志级别
      "appenders": [        //日志输出地
        {
          "type": 0,        //日志输出地类型:0为标准输出地,1为文件输出地,其余非法
          "level": "debug", //日志输出地日志界别,其他日志界别:warn,info,error,fatal
          "format": "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f%l{:}%T%m%n"      //日志格式
        },
        {
          "type": 1,
          "level": "debug",
          "format": "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f%l{:}%T%m%n",
          "filepath": "../resource/log_output/root.txt"                             //文件日志输出路径,标准输出地无路径
        }
      ]
    }
}

*/

//自定义输出地格式
struct LogAppenderDefine{
    int type = 0;
    LogLevel::Level level = LogLevel::DEBUG;
    std::string format = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f%l{:}%T%m%n";
    std::string filepath;
    bool operator==(const LogAppenderDefine rhs) const {
        return type    == rhs.type
             &&level   == rhs.level
             &&format  == rhs.format
             &&filepath== rhs.filepath;
    }
};
//自定义日志器格式
//重载用于stl容器
struct LogDefine{
    std::string name;
    LogLevel::Level level = LogLevel::DEBUG;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& rhs) const {
        return name     == rhs.name
             &&level    == rhs.level
             &&appenders== rhs.appenders;
    }
    bool operator<(const LogDefine& rhs) const {
        return name < rhs.name;
    }

};

//定义LogAppenderDefine和LogDefine的from_json和to_json,详细可查看JSON相关文档<nlohmann/json.hpp>
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogAppenderDefine, type, level, format,filepath)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogDefine, name, level, appenders)

//序列化输出
static std::string LogDefineDump();

}