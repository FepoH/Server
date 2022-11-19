/*
 * @Author: fepo_h
 * @Date: 2022-11-07 11:00:24
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 17:21:47
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_manager.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "log_manager.h"

namespace fepoh{

LogManager::LogManager():m_root(new Logger("root")){
    m_root->addDefaultAppender();
    addLogger(m_root);
}

LogManager::ptr LogManager::GetInstance(){
    static LogManager::ptr log_mgr(new LogManager());
    return log_mgr;
}

bool LogManager::addLogger(Logger::ptr logger){
    MutexLock locker(m_mutex);
    auto it = m_loggers.find(logger->getName());
    if(it == m_loggers.end()){
        m_loggers[logger->getName()] = logger;
        return true;
    }
    return false;
}

void LogManager::delLogger(const std::string& name){
    MutexLock locker(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()){
        m_loggers.erase(it);
    }
}
void LogManager::clrLogger(){
    MutexLock locker(m_mutex);
    m_loggers.clear();
}

Logger::ptr LogManager::getLogger(const std::string& name){
    if(name.empty()){
        return m_root;
    }
    MutexLock locker(m_mutex);
    auto it = m_loggers.find(name);
    if(it == m_loggers.end()){
        Logger::ptr newLogger(new Logger(name));
        m_loggers.insert(std::make_pair(name,newLogger));
        return newLogger;
    }
    return m_loggers[name];
}

bool LogManager::hasLogger(const std::string& name){
    if(name.empty()){
        return false;
    }
    MutexLock locker(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()){
        return true;
    }
    return false;
}

}//namespace