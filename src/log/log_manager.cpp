#include "log_manager.h"



namespace fepoh{

    LogManager::LogManager():m_root(new Logger("root")){
        m_root->addDefaultAppender();
        addLogger(m_root);
    }
    LogManager::~LogManager(){

    }

    LogManager::ptr LogManager::GetInstance(){
        static LogManager::ptr log_mgr(new LogManager());
        return log_mgr;
    }

    bool LogManager::addLogger(Logger::ptr logger){
        MutexLock locker(m_mutex);
        auto it = m_loggers.find(logger->getName());
        if(it == m_loggers.end()){
            m_loggers[logger->getName()] = logger;
        }
        return false;
    }

    void LogManager::delLogger(const std::string& name){
        MutexLock locker(m_mutex);
        auto it = m_loggers.find(name);
        if(it != m_loggers.end()){
            m_loggers.erase(it);
        }
    }
    void LogManager::clrLogger(){
        MutexLock locker(m_mutex);
        m_loggers.clear();
    }

    Logger::ptr LogManager::getLogger(const std::string& name){
        if(name.empty()){
            return m_root;
        }
        MutexLock locker(m_mutex);
        auto it = m_loggers.find(name);
        if(it == m_loggers.end()){
            Logger::ptr newLogger(new Logger(name));
            return newLogger;
        }
        return m_loggers[name];
    }
    

}//namespace