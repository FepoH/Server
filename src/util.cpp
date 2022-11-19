/*
 * @Author: fepo_h
 * @Date: 2022-11-19 21:27:31
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 21:53:00
 * @FilePath: /fepoh/workspace/fepoh_server/src/util.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "util.h"
#include "fiber.h"
#include "thread/thread.h"
#include "log/log.h"

#include <stddef.h>
#include <thread>
#include <sstream>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <execinfo.h>

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

uint32_t GetThreadId(){
    return syscall(SYS_gettid);
}

const std::string& GetThreadName(){
    return Thread::GetName();
}

uint32_t GetFiberId(){
    return fepoh::Fiber::GetFiberId();
}

uint64_t GetCurTimeMs(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

uint64_t GetCurTimeUs(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

std::string ErrorToString(){
    std::stringstream ss;
    ss << "errno = " << errno << ":" << strerror(errno);
    return ss.str();
}

std::string BackTrace(int size,int skip,const std::string& prefix){
    std::vector<std::string> vecStr;
    BackTrace(vecStr,size,skip + 1);
    std::stringstream ss;
    for(auto& item:vecStr){
        ss << prefix << item << std::endl;
    }
    return ss.str();
}

void BackTrace(std::vector<std::string>& vecStr,int size,int skip){
    const int SIZE = size;
    void *buffer[SIZE];
    char **strings;

    int nptrs = backtrace(buffer, SIZE);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        FEPOH_LOG_FATAL(s_log_system) << "backtrace error";
        throw std::logic_error("backtrace error");
    }
    vecStr.resize(nptrs);
    for (int j = skip; j < nptrs; j++){
        vecStr.push_back(strings[j]);
    }
    free(strings);
}


} // namespace fepoh

