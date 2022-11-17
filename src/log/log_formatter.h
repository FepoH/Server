#pragma once

#include "log_level.h"
#include "log_event.h"
#include "thread/mutex.h"

#include <string>
#include <memory>
#include <map>
#include <ostream>
#include <list>

//默认的日志格式
//item的前面必须是%
#define LOG_DEFAULT_FORMAT "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T[%i]%T%m%n"


const size_t LOG_DEFAULT_SIZE = 2048; 

namespace fepoh{
	
	class Logger;
	class LogFormatter{
		public:
			typedef std::shared_ptr<LogFormatter> ptr;
			
			LogFormatter(const std::string& pattern);
			
			void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger);

			class FormatItem{
				public:
					typedef std::shared_ptr<FormatItem> ptr;
					FormatItem(const std::string& addContent):m_addContent(addContent){}
					virtual ~FormatItem(){}
					
					virtual void format(std::ostream& out,LogEvent::ptr event,LogLevel::Level level,std::shared_ptr<Logger> logger) = 0;
				protected:
					std::string m_addContent;
			};
			
			bool getInit() const {return m_isInit;}
		private:
			void init();
			
			Mutex m_mutex;
			bool m_isInit;								//初始化成功
			const std::string m_pattern;				//日志格式字符串
			std::list<FormatItem::ptr> m_items;		//格式组件
	};
	
}//namespace