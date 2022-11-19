/*
 * @Author: fepo_h
 * @Date: 2022-11-19 20:18:54
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 23:52:02
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/thread.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "noncopyable.h"
#include "sem.h"
#include <pthread.h>
#include <functional>
#include <memory>

namespace fepoh{

class Thread : Noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {function<void()>} cb 回调函数
     * @param {string&} m_name 线程名
     */    
    Thread(std::function<void()> cb,const std::string& name);
    ~Thread();
    //join线程
    void join();
    //将线程detach
    void detach();
    //获取线程id
    uint64_t getId() const {return m_id;}
    //获取线程名
    const std::string& getName() const {return m_name;}
    //获取当前线程
    static Thread* GetThis();
    static const std::string& GetName();
    /**
     * @description: 设置当前线程名称
     * @return {*}
     * @param {string&} name 线程名
     */    
    static void SetName(const std::string& name);
private:
    /**
     * @description: 公用的线程调用函数,在此函数内调用cb
     * @return {*}
     * @param {void*} arg 当前thread指针
     */
    static void* Run(void* arg);
private:
    uint64_t m_id = 0;          //线程id
    std::string m_name;         //线程名
    pthread_t m_thread;         //线程
    std::function<void()> m_cb; //回调函数
    Sem m_sem;

};

}