#include "log/log.h"
#include "config.h"

using namespace fepoh;

void test_log(){
    std::cout <<"test_log::start" <<std::endl;
    LogLevel::Level debug = LogLevel::Level::DEBUG;
    Logger::ptr logger(new Logger("system"));
    //LogFormatter::ptr logFmt(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f%l%T%l%n"));
    LogAppender::ptr appender(new FileLogAppender("../bin/log/log.txt"));
    LogAppender::ptr appender1(new StdLogAppender());
    logger->addAppender(appender);
    logger->addAppender(appender1);
    LogEvent::ptr event(new LogEvent("__FILE__",10,0,0,0,time(0),"fepoh"));
    event->getSS()<<"dfgsfgsd I love zls";
    for(int i=0;i<100;++i){
        logger->log(event,LogLevel::Level::ERROR);
    }
    
    std::cout<<"test_log::end"<<std::endl;

}

void test_manager(){
    std::cout <<"test_manager::start" <<std::endl;

    LogEvent::ptr event(new LogEvent("__FILE__",10,0,0,0,time(0),"fepoh"));
    event->getSS()<<"I love zls";


    LogEvent::ptr event1(new LogEvent("__FILE__",10,1111,2222,3333,time(0),"aaa"));
    event1->getSS()<<"I love zls.She is my sunshine";

    LogManager::GetInstance()->getLogger("system")->log(event1,LogLevel::Level::ERROR);
    std::cout<<"test_manager::end"<<std::endl;
}

int main(){
    std::cout << "main begin" <<std::endl;
    //test_log();
    test_manager();
    std::cout << "main::end" <<std::endl;
}
