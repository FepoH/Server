#include "http/http_server.h"
#include "io_manager.h"
#include "log/log.h"

using namespace fepoh;
using namespace fepoh::http;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void run(){
    fepoh::http::HttpServer::ptr serv(new http::HttpServer(true));
    Address::ptr addr = Address::LookupIPAddr("0.0.0.0:8020",0);
    while(!serv->bind(addr)){
        sleep(2);
    }
    auto dp = serv->getDispath();
    dp->addServlet("/zls/xx",[](HttpRequest::ptr request,HttpResponse::ptr response
                        ,HttpSession::ptr session){
        response->setBody(request->tostring());
        std::cout << "====" << response->tostring() << "====" << std::endl;
        return 0;
    });
    dp->addGlobServlet("/zls/*",[](HttpRequest::ptr request,HttpResponse::ptr response
            ,HttpSession::ptr session){
        response->setBody(request->tostring());
        std::cout << "====" << response->tostring() << "====" << std::endl;
        return 0;
    });
    serv->start();
}

int main(){
    IOManager iom;
    iom.schedule(run);
    iom.start();
}