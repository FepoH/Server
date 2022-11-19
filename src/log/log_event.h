/*
 * @Author: fepo_h
 * @Date: 2022-11-10 11:00:11
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 16:48:07
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_event.h
 * @Description: 日志事件
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "log_level.h"
#include <string>
#include <memory>
#include <sstream>
#include <stdarg.h>

namespace fepoh{
class Logger;
/**
 * @description: 日志事件
 * @return {*}
 */
class LogEvent{
public:
	typedef std::shared_ptr<LogEvent> ptr;

	/**
	* @description: 构造函数
	* @return {*}
	* @param {string&} filePath 当前日志文件路径
	* @param {int32_t} line	当前日志行号
	* @param {uint32_t} elapse 已过时间
	* @param {uint32_t} threadId 线程id
	* @param {uint32_t} fiberId 协程id
	* @param {uint64_t} time 当前时间
	* @param {string&} threadName 线程名称
	* @param {string&} funcName 函数名
	*/
	LogEvent(const std::string& filePath,int32_t line,uint32_t elapse
				,uint32_t threadId,uint32_t fiberId,uint64_t time
				,const std::string& threadName,const std::string& funcName = "");
	~LogEvent(){}

	const std::string& getFilePath() const {return m_filePath;}
	int32_t getLine() const {return m_line;}
	uint32_t getElapse() const {return m_elapse;}
	uint32_t getThreadId() const {return m_threadId;}
	uint32_t getFiberId() const {return m_fiberId;}
	uint32_t getTime() const {return m_time;}
	const std::string& getFuncName() const {return m_funcName;}
	const std::string& getThreadName() const {return m_threadName;}
 /**
  * @description: 获取日志内容流,使日志支持流
  * @return {*}
  */
	std::stringstream& getSS(){return m_ss;}
private:
	std::string m_filePath;			//文件名
	int32_t m_line;					//行号
	uint32_t m_elapse;				//程序启动开始到现在的毫秒数
	uint32_t m_threadId;			//线程ID
	uint32_t m_fiberId;				//协程ID
	uint64_t m_time;				//时间
	std::string m_funcName;			//函数名
	std::string m_threadName;		//线程名字
	std::stringstream m_ss;			//日志内容字符流
};
/**
 * @description: 日志事件包装器
 * @return {*}
 */
class LogEventWrap{
public:
	LogEventWrap(LogEvent::ptr event,std::shared_ptr<Logger> logger,LogLevel::Level level);
 /**
  * @description: 通过析构函数完成日志输出
  * @return {*}
  */	
	~LogEventWrap();
 /**
  * @description: 获取日志器流
  * @return {*}
  */
	std::stringstream& getSS(){return m_event->getSS();}
 /**
  * @description: 格式化日志输出
  * @return {*}
  * @param {char*} fmt
  */	
	void format(const char* fmt,...);
	void format(const char* fmt,va_list al);
private:
	LogEvent::ptr m_event;				//日志事件
	std::shared_ptr<Logger> m_logger;	//日志器
	LogLevel::Level m_level;			//日志级别
};

	
}//namespace