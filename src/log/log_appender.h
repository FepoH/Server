#pragma once

#include "log_formatter.h"
#include "log_level.h"
#include "thread/mutex.h"

#include <string>
#include <memory>
#include <fstream>
/*
	日志输出地类
*/

namespace fepoh{
	
	class Logger;
	class LogAppender{
		public:
			typedef std::shared_ptr<LogAppender> ptr;
			enum AppenderType{
				STD_OUT = 0,
				FILE_OUT
			};

			LogAppender(LogFormatter::ptr formatter,LogLevel::Level level);

			virtual ~LogAppender(){}
			//日志输出的主要函数
			virtual void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) = 0;
			
			LogFormatter::ptr getFormatter() const {return m_formatter;}
			LogLevel::Level getLevel() const {return m_level;}
			
			void setFormatter(LogFormatter::ptr formatter) {m_formatter = formatter;}
			void setLevel(LogLevel::Level level) {m_level = level;}
			
		protected:
			Mutex m_mutex;
			LogFormatter::ptr m_formatter;	//日志格式
			LogLevel::Level m_level;		//日志级别
	};
	
	 
	class StdLogAppender : public LogAppender{
		public:
			typedef std::shared_ptr<StdLogAppender> ptr;
		
			StdLogAppender(LogFormatter::ptr formatter = LogFormatter::ptr(new LogFormatter(LOG_DEFAULT_FORMAT)),LogLevel::Level level = LogLevel::Level::DEBUG);
			
			
			void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override;
	};
	
	class FileLogAppender : public LogAppender{
		public:
			typedef std::shared_ptr<FileLogAppender> ptr;
			
			FileLogAppender(const std::string& filepath,LogFormatter::ptr formatter= LogFormatter::ptr(new LogFormatter(LOG_DEFAULT_FORMAT)),LogLevel::Level level = LogLevel::Level::DEBUG);
			
			~FileLogAppender(){}
			
			

			void log(LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) override;
			//设置文件路径
			void setFilePath(const std::string& filepath);
			
		private:
			bool reopen();

			std::ofstream m_filestream;			//文件输出流
			uint64_t m_lastOpenTime = 0;		//文件打开时间,经过一段时间文件重打开
			std::string m_filepath;				//文件路径
	};
	
}//namespace