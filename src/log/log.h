#pragma once

#include "logger.h"
#include "log_manager.h"
#include "util.h"


// LogEvent::LogEvent(const std::string& filePath,int32_t line,uint32_t elapse
// 						,uint32_t threadId,uint32_t fiberId,uint64_t time,const std::string& threadName)


/*
    日志的所有类可以集合在一个文件中,笔者有意分开玩一玩,建议放在一起,这样更加方便
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
//格式为:FEPOH_LOG_DEBUG1(logger,"i = %d ,str = %s",10,"I love you three thousand times");
#define FEPOH_LOG_DEBUG1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::DEBUG,__VA_ARGS__)
#define FEPOH_LOG_INFO1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::INFO,__VA_ARGS__)
#define FEPOH_LOG_WARN1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::WARN,__VA_ARGS__)
#define FEPOH_LOG_ERROR1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::ERROR,__VA_ARGS__)
#define FEPOH_LOG_FATAL1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::FATAL,__VA_ARGS__)

#define FEPOH_LOG_NAME(name)    \
    fepoh::LogManager::GetInstance()->getLogger(name)


namespace fepoh{

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

//定义LogAppenderDefine和LogDefine的from_json和to_json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogAppenderDefine, type, level, format,filepath)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogDefine, name, level, appenders)

static std::string LogDefineDump();

}