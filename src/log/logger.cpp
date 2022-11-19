/*
 * @Author: fepo_h
 * @Date: 2022-11-19 14:24:08
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 15:29:02
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/logger.cpp
 * @Description: 日志器类
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "logger.h"
#include "log_formatter.h"
#include "log_manager.h"

namespace fepoh{

Logger::Logger(const std::string& name,LogLevel::Level level)
		:m_name(name),m_level(level){
}

void Logger::addAppender(LogAppender::ptr appender){
	if(appender){
		MutexLock locker(m_mutex);
		m_appenders.push_back(appender);
	}
}

void Logger::clrAppender(){
	MutexLock locker(m_mutex);
	m_appenders.clear();
}

void Logger::delAppender(LogAppender::ptr appender){
	if(appender){
		for(auto it = m_appenders.begin();it!=m_appenders.end();++it){
			if(*it==appender){
				MutexLock locker(m_mutex);
				m_appenders.erase(it);
				break;
			}
		}
	}
}

void Logger::addDefaultAppender(){
	LogAppender::ptr stdLogAppender(new StdLogAppender());
	MutexLock locker(m_mutex);
	m_appenders.push_back(stdLogAppender);
}

void Logger::log(LogEvent::ptr logEvent,LogLevel::Level level){
	if(level >= m_level){
		//通过shared_from_this(),当日志输出未完成时,保证此日志器不会释放
		Logger::ptr self = shared_from_this();
		for(auto it = m_appenders.begin();it != m_appenders.end();++it){
			MutexLock locker(m_mutex);
			(*it)->log(logEvent,level,self);
		}
		//日志器为空,将主日志器和标准输出地作为输出,以此保证有最基本的日志输出
		if(m_appenders.empty()){
			Logger::ptr root = LogManager::GetInstance()->getRoot()->shared_from_this();
			for(auto it = root->m_appenders.begin();it != root->m_appenders.end();++it){
				MutexLock locker(m_mutex);
				(*it)->log(logEvent,level,root);
			}
		}
	}
}
	
}//namespace