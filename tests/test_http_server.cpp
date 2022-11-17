#include "http/http_server.h"
#include "io_manager.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void run(){
    fepoh::http::HttpServer::ptr serv(new http::HttpServer());
    Address::ptr addr = Address::LookupIPAddr("0.0.0.0:8033",0);
    while(!serv->bind(addr)){
        sleep(2);
    }
    serv->start();
}

int main(){
    IOManager iom;
    iom.schedule(run);
    iom.start();
}