#pragma once

#include "log_level.h"
#include "log_appender.h"

#include <string>
#include <memory>
#include <list>


namespace fepoh{
	
	class LogManager;
	class Logger : public std::enable_shared_from_this<Logger>{
		public:
			friend class LogManager;
			typedef std::shared_ptr<Logger> ptr; 
			Logger(const std::string& name,LogLevel::Level level = LogLevel::Level::DEBUG);
			~Logger(){}
			
			//添加输出地
			void addAppender(LogAppender::ptr appender);
			//清空输出地
			void clrAppender();
			//删除输出地
			void delAppender(LogAppender::ptr appender);
			//添加标准输出地
			void addDefaultAppender();
			
			void setLevel(LogLevel::Level level){m_level = level;}
			LogLevel::Level getLevel()const {return m_level;}
			const std::string& getName()const{return m_name;}
			
			//主要日志输出函数
			void log(LogEvent::ptr logEvent,LogLevel::Level level);
		
		private:
			Mutex m_mutex;
			const std::string m_name;					//日志器名称
			LogLevel::Level m_level;					//日志级别
			std::list<LogAppender::ptr>	m_appenders;	//日志输出地集合
	};
	
	
	
}//namesapce