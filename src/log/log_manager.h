/*
 * @Author: fepo_h
 * @Date: 2022-10-30 15:21:10
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 17:21:21
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_manager.h
 * @Description: 日志管理类
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "logger.h"
#include "thread/mutex.h"

#include <boost/noncopyable.hpp>
#include <map>


namespace fepoh{
    
class LogManager : public boost::noncopyable{
public:
    typedef std::shared_ptr<LogManager> ptr;

    static LogManager::ptr GetInstance();
    /**
     * @description: 获取root日志器
     * @return {*}
     */
    Logger::ptr getRoot(){return m_root;}
    //添加logger
    bool addLogger(Logger::ptr logger);
    //删除logger
    void delLogger(const std::string& name);
    //清空logger
    void clrLogger();
    //如果没有,新创建一个,并加入管理器
    Logger::ptr getLogger(const std::string& name);
    //是否有该logger
    bool hasLogger(const std::string& name);

private:
    /**
     * @description: 单例模式
     * @return {*}
     */
    LogManager();

    Mutex m_mutex;
    std::map<std::string,Logger::ptr> m_loggers;
    Logger::ptr m_root;
};



} // namespace fepoh
