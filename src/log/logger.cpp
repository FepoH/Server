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
			Logger::ptr self = shared_from_this();
			for(auto it = m_appenders.begin();it != m_appenders.end();++it){
				MutexLock locker(m_mutex);
				(*it)->log(logEvent,level,self);
			}
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