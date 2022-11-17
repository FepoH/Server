#pragma once

#include "io_manager.h"
#include "socket_.h"
#include "address.h"
#include "noncopyable.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>

namespace fepoh{


class TcpServer : public std::enable_shared_from_this<TcpServer>
                , public Noncopyable{
public:
    typedef std::shared_ptr<TcpServer> ptr;

    TcpServer(fepoh::IOManager* worker = fepoh::IOManager::GetThis(),
                fepoh::IOManager* accept_worker = fepoh::IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(fepoh::Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr>& addrs,std::vector<Address::ptr>& failAddrs);
    virtual bool start();
    virtual void stop();

    uint64_t getReadTimeout() const {return m_readTimeout;}
    std::string getName() const {return m_name;}
    void setName(const std::string& v) {m_name = v;}
    void setReadTimeout(uint64_t v) {m_readTimeout = v;}

    bool isStop() const {return m_isStop;}
protected:
    virtual void handleClient(Socket::ptr client);
    virtual void startAccept(Socket::ptr sock);

private:
    //同时监听多地址
    std::vector<Socket::ptr> m_socks;
    IOManager* m_worker;    //处理连接等
    IOManager* m_acceptWorker;    //处理连接等
    uint64_t m_readTimeout; //处理非活跃连接和非法连接等
    std::string m_name;       //方便调试和日志输出
    bool m_isStop;



};




}//namesapce