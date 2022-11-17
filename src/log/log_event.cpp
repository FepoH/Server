#include "log_event.h"
#include "logger.h"

namespace fepoh{
	
	LogEvent::LogEvent(const std::string& filePath,int32_t line,uint32_t elapse
						,uint32_t threadId,uint32_t fiberId,uint64_t time
						,const std::string& threadName,const std::string& funcName)
			:m_filePath(filePath),m_line(line),m_elapse(elapse),m_threadId(threadId)
			,m_fiberId(fiberId),m_time(time),m_threadName(threadName),m_funcName(funcName){
		m_ss.str("");			
	}

	LogEvent::~LogEvent(){}
	
	LogEventWrap::LogEventWrap(LogEvent::ptr event,std::shared_ptr<Logger> logger,LogLevel::Level level)
			:m_event(event),m_logger(logger),m_level(level){
		
	}
	
	LogEventWrap::~LogEventWrap(){
		m_logger->log(m_event,m_level);
	}
	
	void LogEventWrap::format(const char* fmt,...){
		va_list al;
		va_start(al,fmt);
		format(fmt,al);
		va_end(al);
	}
	
	void LogEventWrap::format(const char* fmt,va_list al){
		char buf[LOG_DEFAULT_SIZE];
		int len = vsnprintf(buf,LOG_DEFAULT_SIZE,fmt,al);
		//int len = vasnprintf(&buf,fmt,al);
		if(len != -1){
			(m_event->getSS())<< std::string(buf,len);
		}
	}
	
}//namespace

//vasnprintf
//vsnprintf

// 可变参数
// void LogEvent::format(const char* fmt, ...) {
    // va_list al;
    // va_start(al, fmt);
    // format(fmt, al);
    // va_end(al);
// }

// void LogEvent::format(const char* fmt, va_list al) {
    // char* buf = nullptr;
    // int len = vasprintf(&buf, fmt, al);
    // if(len != -1) {
        // m_ss << std::string(buf, len);
        // free(buf);
    // }
// }