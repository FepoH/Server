/*
 * @Author: fepo_h
 * @Date: 2022-11-19 15:06:30
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 15:13:52
 * @FilePath: /fepoh/workspace/fepoh_server/src/log/log_formatter.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "log_level.h"
#include "log_event.h"
#include "thread/mutex.h"

#include <string>
#include <memory>
#include <map>
#include <ostream>
#include <list>

/**
 * @description: 日志格式,详见log.h
 */
#define LOG_DEFAULT_FORMAT "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T[%i]%T%m%n"


const size_t LOG_DEFAULT_SIZE = 2048; 

namespace fepoh{

class Logger;
/**
 * @description: 日志格式
 */
class LogFormatter{
public:
	typedef std::shared_ptr<LogFormatter> ptr;
	
	LogFormatter(const std::string& pattern);
	/**
	 * @description: 日志主要流程函数
	 * @return {*}
	 * @param {ostream&} out 输出地
	 * @param {ptr} event 日志事件
	 * @param {Level} level 日志级别
	 * @param {shared_ptr<Logger>} logger 日志器
	 */	
	void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger);

	/**
	 * @description: 日志格式组件,基类
	 * @return {*}
	 */
	class FormatItem{
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			/**
			 * @description: 构造函数
			 * @return {*}
			 * @param {string&} addContent 附件字符串输出
			 */
			FormatItem(const std::string& addContent):m_addContent(addContent){}
			virtual ~FormatItem(){}
			
			/**
			 * @description: 日志输出主要流程函数
			 * @return {*}
			 * @param {ostream&} out 输出地
			 * @param {ptr} event 日志事件
			 * @param {Level} level 日志级别
			 * @param {shared_ptr<Logger>} logger 日志器
			 */
			virtual void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) = 0;
		protected:
			std::string m_addContent;	//附加内容
	};
	
	bool getInit() const {return m_isInit;}
private:
	void init();	//日志格式初始化
	
	Mutex m_mutex;
	bool m_isInit;							//是否初始化
	const std::string m_pattern;			//日志格式字符串
	std::list<FormatItem::ptr> m_items;		//格式组件
};
	
}//namespace