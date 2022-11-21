/*
 * @Author: fepo_h
 * @Date: 2022-11-20 20:31:21
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:31:22
 * @FilePath: /fepoh/workspace/fepoh_server/src/stream.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <string>
#include "bytearray.h"

namespace fepoh{

class Stream{
public:
    typedef std::shared_ptr<Stream> ptr;
    virtual ~Stream(){}
    //读数据
    virtual int read(void* buffer,size_t length) = 0;
    virtual int read(ByteArray::ptr ba,size_t length) = 0;
    //必须读到一定长度
    virtual int readFixSize(void* buffer,size_t length);
    virtual int readFixSize(ByteArray::ptr ba,size_t length);

    //写数据
    virtual int write(const void* buffer,size_t length) = 0;
    virtual int write(ByteArray::ptr ba,size_t length) = 0;
    //必须写到一定长度
    virtual int writeFixSize(const void* buffer,size_t length);
    virtual int writeFixSize(ByteArray::ptr ba,size_t length);

    virtual void close() = 0;
};


}//namespace