#include "http_server.h"
#include "log/log.h"

namespace fepoh{
namespace http{
static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive, fepoh::IOManager* worker,IOManager* accept_worker)
        :TcpServer(worker,accept_worker)
        ,m_isKeepalive(keepalive)
        ,m_dispath(new ServletDispath()){
    
}

void HttpServer::handleClient(Socket::ptr client) {
    HttpSession::ptr session(new HttpSession(client));
    do{
        auto req = session->recvRequrest();
        if(!req){
            FEPOH_LOG_WARN(s_log_system) << "recv http request fail.errno = "
                    << errno << ":" << strerror(errno);
            break; 
        }
        HttpResponse::ptr rsp(new HttpResponse(req->getVersion(),req->getClose() || !m_isKeepalive));
        m_dispath->handle(req,rsp,session);
        //rsp->setBody("hello darling-ls");
        session->sendResponse(rsp);
    }while(m_isKeepalive);
    session->close();
}
//220.202.232.102

}

}//namespace