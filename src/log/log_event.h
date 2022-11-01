#pragma once

#include "log_level.h"
#include <string>
#include <memory>
#include <sstream>
#include <stdarg.h>

namespace fepoh{
	class Logger;
	class LogEvent{
		public:
			typedef std::shared_ptr<LogEvent> ptr;
		
			LogEvent(const std::string& filePath,int32_t line,uint32_t elapse
						,uint32_t threadId,uint32_t fiberId,uint64_t time,const std::string& threadName);
			~LogEvent();

			const std::string& getFilePath() const {return m_filePath;}
			int32_t getLine() const {return m_line;}
			uint32_t getElapse() const {return m_elapse;}
			uint32_t getThreadId() const {return m_threadId;}
			uint32_t getFiberId() const {return m_fiberId;}
			uint32_t getTime() const {return m_time;}
			const std::string& getThreadName() const {return m_threadName;}
			//此处支持流式输出日志
			std::stringstream& getSS(){return m_ss;}
		private:
			std::string m_filePath;				//文件名
			int32_t m_line;					//行号
			uint32_t m_elapse;				//程序启动开始到现在的毫秒数
			uint32_t m_threadId;			//线程ID
			uint32_t m_fiberId;				//协程ID
			uint64_t m_time;				//时间
			std::string m_threadName;		//线程名字
			std::stringstream m_ss;			//日志内容字符流
	};
	
	class LogEventWrap{
		public:
			LogEventWrap(LogEvent::ptr event,std::shared_ptr<Logger> logger,LogLevel::Level level);
			//通过析构函数进行日志输出
			~LogEventWrap();
		
			std::stringstream& getSS(){return m_event->getSS();}
			
			void format(const char* fmt,...);
			void format(const char* fmt,va_list al);
		private:
			LogEvent::ptr m_event;
			std::shared_ptr<Logger> m_logger;
			LogLevel::Level m_level;
	};
	
	
}//namespace