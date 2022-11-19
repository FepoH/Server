/*
 * @Author: fepo_h
 * @Date: 2022-11-19 14:06:59
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 15:42:57
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log.h
 * @Description: 日志部分整合,增加日志配置模块
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "logger.h"
#include "log_manager.h"
#include "util.h"
#include <nlohmann/json.hpp>


/*
    注:日志的所有类可以集合在一个文件中,本人有意分开试一下,建议放在一起,这样更加方便
    流式输出:
      FEPOH_LOG_DEBUG(logger) << "I love you three thousand";
    格式化输出:
      FEPOH_LOG_DEBUG1(logger,"%s%d,%s","I love you ",3000,"Miss Zhou");

    日志的配置文件完成配置文件转换,格式如下:

    {
      "logs": [
        {
          "name": "root",       //日志器名称
          "level": "debug",     //日志器日志级别
          "appenders": [        //日志输出地
            {
              "type": 0,        //日志输出地类型:0为标准输出地,1为文件输出地,其余非法
              "level": "debug", //日志输出地日志界别,其他日志界别:warn,info,error,fatal
              "format": "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T[%i]%T%m%n"  //日志格式
            },
            {
              "type": 1,
              "level": "debug",
              "format": "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T[%i]%T%m%n",
              "filepath": "../resource/log_output/root.txt"  //文件日志输出路径,标准输出地无路径
            }
          ]
        }
      ]
    }

*/

//流式输出
#define FEPOH_LOG_LEVEL(logger,level)   \
    if(level>=logger->getLevel())        \
        fepoh::LogEventWrap(fepoh::LogEvent::ptr(new fepoh::LogEvent(__FILE__,__LINE__,-1,fepoh::GetThreadId(),fepoh::GetFiberId(),::time(0),fepoh::GetThreadName(),__FUNCTION__)),logger,level).getSS()

#define FEPOH_LOG_DEBUG(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::DEBUG)
#define FEPOH_LOG_INFO(logger)  FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::INFO)
#define FEPOH_LOG_WARN(logger)  FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::WARN)
#define FEPOH_LOG_ERROR(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::ERROR)
#define FEPOH_LOG_FATAL(logger) FEPOH_LOG_LEVEL(logger,fepoh::LogLevel::Level::FATAL)

//格式化输出
#define FEPOH_LOG_FORMAT(logger,level,...)  \
    if(level>=logger->getLevel())           \
        fepoh::LogEventWrap(fepoh::LogEvent::ptr(new fepoh::LogEvent(__FILE__,__LINE__,-1,fepoh::GetThreadId(),fepoh::GetFiberId(),time(0),"fepoh",__FUNCTION__)),logger,level).format(__VA_ARGS__);


#define FEPOH_LOG_DEBUG1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::DEBUG,__VA_ARGS__)
#define FEPOH_LOG_INFO1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::INFO,__VA_ARGS__)
#define FEPOH_LOG_WARN1(logger,...)  FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::WARN,__VA_ARGS__)
#define FEPOH_LOG_ERROR1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::ERROR,__VA_ARGS__)
#define FEPOH_LOG_FATAL1(logger,...) FEPOH_LOG_FORMAT(logger,fepoh::LogLevel::Level::FATAL,__VA_ARGS__)
//获取日志器
#define FEPOH_LOG_NAME(name)    \
    fepoh::LogManager::GetInstance()->getLogger(name)


namespace fepoh{

/**
 * @description: 自定义输出地格式
 * @return {*}
 * @param type 日志输出地类型
 * @param level 日志级别
 * @param format 日志格式
 * @param filepath 文件路径
 */
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
/**
 * @description: 自定义日志器器格式
 * @return {*}
 * @param name 日志器名
 * @param level 日志级别
 * @param appenders 日志输出地
 */
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

/**
 * @description: 自定义格式转字符串,方便debug 
 * @return {*}
 */
static std::string LogDefineDump(const LogDefine& ld);

}