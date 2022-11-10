#pragma once

#include "thread/mutex.h"
#include "io_manager.h"

#include <vector>
#include <memory>

namespace fepoh{

class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    typedef std::shared_ptr<FdCtx> ptr;

    FdCtx(int fd);
    ~FdCtx();  

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

    IOManager* getIOManager() {return m_iomanager;}
    void setIOManager(IOManager* val) {m_iomanager = val;}

    uint64_t getTimeout(int type);
    void setTimeout(int type, uint64_t timeout);

private:
    bool m_isInit;          //是否初始化
    bool m_isSocket;        //是否是socket文件描述符
    bool m_sysNonblock;      //是否是系统设置的非阻塞
    bool m_userNonblock;     //是否是用户设置的非阻塞
    bool m_isClose;        //是否已经关闭
    int m_fd;               //文件描述符
    uint64_t m_recvTimeout; //接收超时时间
    uint64_t m_sendTimeout; //发送超时时间
    fepoh::IOManager* m_iomanager; 
};

class FdManager{
public:
    typedef std::shared_ptr<FdManager> ptr;

    static FdManager::ptr GetInstance();

    FdCtx::ptr get(int fd,bool auto_create = false);
    void del(int fd);
private:
    FdManager();

private:
    RWMutex m_mutex;
    std::vector<FdCtx::ptr> m_fds;

};


}//namespace