/*
 * @Author: fepo_h
 * @Date: 2022-11-20 17:45:07
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 18:00:33
 * @FilePath: /fepoh/workspace/fepoh_server/src/fd_manager.h
 * @Description: 文件描述符管理类
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "thread/mutex.h"
#include "io_manager.h"

#include <vector>
#include <memory>

namespace fepoh{

class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    typedef std::shared_ptr<FdCtx> ptr;
    /**
     * @description: 构造函数
     * @param {int} fd 文件描述符
     */    
    FdCtx(int fd);
    /**
     * @description: 析构函数
     */    
    ~FdCtx();  
    /**
     * @description: 初始化
     * @return {*}
     */    
    bool init();
    bool isInit() const {return m_isInit;}
    bool isClose() const {return m_isClose;}
    bool isSocket() const {return m_isSocket;}

    bool getSysNonblock() const {return m_sysNonblock;}
    void setSysNonblock(bool val) {m_sysNonblock = val;}

    bool getUserNonblock() const {return m_userNonblock;}
    void setUserNonblock(bool val) {m_userNonblock = val;} 

    int getFd() const {return m_fd;}

    uint64_t getRecvTimeout() const {return m_recvTimeout;}
    void setRecvTimeout(uint64_t val) {m_recvTimeout = val;}

    uint64_t getSendTimeout() const {return m_sendTimeout;}
    void setSendTimeout(uint64_t val) {m_sendTimeout = val;}

    /**
     * @description: 获取当前IOManager
     * @return {*}
     */    
    IOManager* getIOManager() {return m_iomanager;}
    /**
     * @description: 设置当前IOManager
     * @return {*}
     * @param {IOManager*} val
     */    
    void setIOManager(IOManager* val) {m_iomanager = val;}

    uint64_t getTimeout(int type);
    void setTimeout(int type, uint64_t timeout);
private:
    bool m_isInit;              //是否初始化
    bool m_isSocket;            //是否是socket文件描述符
    bool m_sysNonblock;         //是否是系统设置的非阻塞
    bool m_userNonblock;        //是否是用户设置的非阻塞
    bool m_isClose;             //是否已经关闭
    int m_fd;                   ///文件描述符
    uint64_t m_recvTimeout;     ///接收超时时间
    uint64_t m_sendTimeout;     ///发送超时时间
    fepoh::IOManager* m_iomanager; 
};

class FdManager{
public:
    typedef std::shared_ptr<FdManager> ptr;

    static FdManager::ptr GetInstance();
    /**
     * @description: 获取FdCtx
     * @return {*}
     * @param {int} fd  文件描述符
     * @param {bool} auto_create 若文件描述符不存在,是否自动创建
     */    
    FdCtx::ptr get(int fd,bool auto_create = false);
    void del(int fd);
private:
    FdManager();

private:
    RWMutex m_mutex;
    std::vector<FdCtx::ptr> m_fds;

};


}//namespace