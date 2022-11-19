#include "src/http/http_server.h"
#include "io_manager.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("root");

void run(){
    s_log_system->setLevel(LogLevel::ERROR);
    Address::ptr addr = Address::LookupIPAddr("0.0.0.0:8020",0);
    if(!addr){
        FEPOH_LOG_ERROR(s_log_system) << "invalid addr";
        return ;
    }
    http::HttpServer::ptr server(new http::HttpServer(true));
    while(!server->bind(addr)){
        sleep(1);
    }
    server->start();
}

void load_log(){
    
}


int main(){
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");
    load_log();
    IOManager iom("",2);
    iom.schedule(run);
    iom.start();
}