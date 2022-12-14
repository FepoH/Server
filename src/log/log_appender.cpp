/*
 * @Author: fepo_h
 * @Date: 2022-11-19 14:33:07
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 17:01:24
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_appender.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "log_appender.h"
#include "logger.h"

#include <iostream>
#include <time.h>

namespace fepoh{

LogAppender::LogAppender(LogFormatter::ptr formatter,LogLevel::Level level)
		:m_formatter(formatter),m_level(level){
}

StdLogAppender::StdLogAppender(LogFormatter::ptr formatter,LogLevel::Level level)
		:LogAppender(formatter,level){
}

void StdLogAppender::log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) {
	if(level>=m_level){
		MutexLock locker(m_mutex);
		m_formatter->format(std::cout,event,level,logger);
	}
}

FileLogAppender::FileLogAppender(const std::string& filepath,LogFormatter::ptr formatter,LogLevel::Level level)
		:LogAppender(formatter,level),m_filepath(filepath){
	m_lastOpenTime = time(0);
	reopen();
}

//设置文件路径,当重新设置的文件路径打开失败时,采用以前的文件路径
void FileLogAppender::setFilePath(const std::string& filepath){
	if(filepath == ""){
		return ;
	}
	MutexLock locker(m_mutex);
	m_filepath = filepath;
	reopen();
}

bool FileLogAppender::reopen(){
	if(m_filestream){
		m_filestream.close();
	}
	m_filestream.open(m_filepath,std::ios_base::out|std::ios_base::app);
	if(m_filestream.is_open()){
		return true;
	}
	return false;
}

void FileLogAppender::log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) {
	bool flag = false;
	if((time(0)-m_lastOpenTime)>1){
		MutexLock locker(m_mutex);
		flag = reopen();
		if(!flag){
			std::cout<<"logappender error:file open fail."<<std::endl;
			return ;
		}
	}
	if(level >= m_level){
		MutexLock locker(m_mutex);
		m_formatter->format(m_filestream,event,level,logger);
	}
}

	
}//namespace