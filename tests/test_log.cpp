/*
 * @Author: fepo_h
 * @Date: 2022-11-19 16:39:05
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 17:26:29
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_log.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "log/log.h"
#include "config.h"

using namespace fepoh;

void test_log(){
    std::cout <<"test_log::start" <<std::endl;
    LogLevel::Level debug = LogLevel::Level::INFO;
    Logger::ptr logger(new Logger("test_log",LogLevel::Level::INFO));
    LogFormatter::ptr logFmt(new LogFormatter("%d{%H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f%l%T%l%n"));
    LogAppender::ptr appender(new FileLogAppender("/home/fepoh/workspace/fepoh_server/bin/log/test_log.txt",logFmt));
    LogAppender::ptr appender1(new StdLogAppender(logFmt));
    logger->addAppender(appender);
    logger->addAppender(appender1);
    LogEvent::ptr event(new LogEvent("__FILE__",10,0,0,0,0,"fepoh"));
    event->getSS()<<"dfgsfgsd I love zls";
    for(int i=0;i<10;++i){
        logger->log(event,LogLevel::Level::ERROR);
    }
    std::cout<<"test_log::end"<<std::endl;

}
static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_config(){
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");

    FEPOH_LOG_WARN(s_log_system) << "i love you 3000";
}



int main(){
    std::cout << "main begin" <<std::endl;
    test_log();
    test_config();
    std::cout << "main::end" <<std::endl;
}
