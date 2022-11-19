#pragma once

#include "http/http_session.h"
#include "tcp_server.h"
#include "http.h"
#include "servlet.h"

namespace fepoh{
namespace http{
class HttpServer : public TcpServer{
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false, fepoh::IOManager* worker = IOManager::GetThis(),
                IOManager* accept_worker = fepoh::IOManager::GetThis());

    ServletDispath::ptr getDispath() {return m_dispath;}
    void setServletDispath(ServletDispath::ptr v) {m_dispath = v;}
protected:
    virtual void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive = false;
    ServletDispath::ptr m_dispath;
};

}
}//namespace