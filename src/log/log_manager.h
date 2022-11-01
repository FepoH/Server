#pragma once

#include "logger.h"
#include "thread/mutex.h"

#include <boost/noncopyable.hpp>
#include <map>


namespace fepoh{
    
    class LogManager : public boost::noncopyable{
        public:
            typedef std::shared_ptr<LogManager> ptr;

            ~LogManager();

            static LogManager::ptr GetInstance();

            Logger::ptr getRoot(){return m_root;}

            bool addLogger(Logger::ptr logger);

            void delLogger(const std::string& name);
            void clrLogger();
            //如果没有找到,默认返回m_root
            Logger::ptr getLogger(const std::string& name);

        private:
            LogManager();

            void init();

            Mutex m_mutex;
            std::map<std::string,Logger::ptr> m_loggers;
            Logger::ptr m_root;
    };



} // namespace fepoh
