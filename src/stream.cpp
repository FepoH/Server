/*
 * @Author: fepo_h
 * @Date: 2022-11-17 17:33:24
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:37:40
 * @FilePath: /fepoh/workspace/fepoh_server/src/stream.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "stream.h"
#include "log/log.h"

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

int Stream::readFixSize(void* buffer,size_t length){
    int left = length;
    int offset = 0;
    while(left > 0 ){
        int rt = read((char*)buffer + offset,left);
        if(rt <=0 ){
            FEPOH_LOG_ERROR(s_log_system) << "readFixSize error.errno = " << errno
                    << ":" << strerror(errno); 
            return rt;
        }
        left -= rt;
        offset += rt;
    }
    return length;
}

int Stream::readFixSize(ByteArray::ptr ba,size_t length){
    int left = length;
    while(left > 0 ){
        int rt = read(ba,left);
        if(rt <=0 ){
            FEPOH_LOG_ERROR(s_log_system) << "readFixSize error.errno = " << errno
                    << ":" << strerror(errno); 
            return rt;
        }
        left -= rt;
    }
    return length;
}

int Stream::writeFixSize(const void* buffer,size_t length){
    int left = length;
    int offset = 0;
    while(left > 0 ){
        int rt = write((char*)buffer + offset,left);
        if(rt <=0 ){
            FEPOH_LOG_ERROR(s_log_system) << "writeFixSize error.errno = " << errno
                    << ":" << strerror(errno); 
            return rt;
        }
        left -= rt;
        offset += rt;
    }
    return length;
}

int Stream::writeFixSize(ByteArray::ptr ba,size_t length){
    int left = length;
    while(left > 0 ){
        int rt = write(ba,left);
        if(rt <=0 ){
            FEPOH_LOG_ERROR(s_log_system) << "writeFixSize error.errno = " << errno
                    << ":" << strerror(errno); 
            return rt;
        }
        left -= rt;
    }
    return length;
}


}//namespace