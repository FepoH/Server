/*
 * @Author: fepo_h
 * @Date: 2022-11-01 19:36:25
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 14:43:31
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_appender.h
 * @Description: 日志输出地
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "log_formatter.h"
#include "log_level.h"
#include "thread/mutex.h"

#include <string>
#include <memory>
#include <fstream>

namespace fepoh{
	
class Logger;
class LogAppender{
public:
	typedef std::shared_ptr<LogAppender> ptr;
	//输出地类型
	enum class AppenderType{
		STD_OUT = 0,	//标准输出
		FILE_OUT		//文件输出
	};
	/**
	 * @description: 构造函数
	 * @return {*}
	 * @param {ptr} formatter 日志格式
	 * @param {Level} level 日志级别
	 */	
	LogAppender(LogFormatter::ptr formatter,LogLevel::Level level);

	virtual ~LogAppender(){}
	/**
	 * @description: 日志输出主要函数
	 * @return {*}
	 * @param {ptr} event 日志事件
	 * @param {Level} level 日志级别
	 * @param {shared_ptr<Logger>} logger 日志器
	 */	
	virtual void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) = 0;
	
	LogFormatter::ptr getFormatter() const {return m_formatter;}
	LogLevel::Level getLevel() const {return m_level;}
	
	void setFormatter(LogFormatter::ptr formatter) {m_formatter = formatter;}
	void setLevel(LogLevel::Level level) {m_level = level;}
	
protected:
	Mutex m_mutex;					//互斥锁
	LogFormatter::ptr m_formatter;	//日志格式
	LogLevel::Level m_level;		//日志级别
};


class StdLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<StdLogAppender> ptr;

	StdLogAppender(LogFormatter::ptr formatter = LogFormatter::ptr(new LogFormatter(LOG_DEFAULT_FORMAT))
								,LogLevel::Level level = LogLevel::Level::DEBUG);
	
	
	void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override;
};

class FileLogAppender : public LogAppender{
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	
	FileLogAppender(const std::string& filepath
			,LogFormatter::ptr formatter= LogFormatter::ptr(new LogFormatter(LOG_DEFAULT_FORMAT))
			,LogLevel::Level level = LogLevel::Level::DEBUG);
	
	void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override;
	//设置文件路径
	void setFilePath(const std::string& filepath);
	const std::string& getFilePath() const {return m_filepath;}
	
private:
	/**
	 * @description: 文件重打开,当文件被删除或路径改变时调用
	 * @return {*}
	 */	
	bool reopen();

	std::ofstream m_filestream;			//文件输出流
	uint64_t m_lastOpenTime = 0;		//文件打开时间,经过一段时间文件重打开
	std::string m_filepath;				//文件路径
};
	
}//namespace