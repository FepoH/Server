#include "tcp_server.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test(){
    Address::ptr addr1 = Address::LookupIPAddr("0.0.0.0:8033",0);
    Address::ptr addr2(new UnixAddress("/tmp/unix_addr"));
    FEPOH_LOG_DEBUG(s_log_system) << addr1->tostring() << " - " << addr2->tostring();

    TcpServer::ptr tcp_server(new TcpServer());
    std::vector<Address::ptr> addrs;
    addrs.push_back(addr1);
    addrs.push_back(addr2);
    std::vector<Address::ptr> failAddrs;
    while(!tcp_server->bind(addrs,failAddrs)){
        sleep(2);
    }
    tcp_server->start();

}

int main(){
    IOManager iom;
    iom.schedule(test);
    iom.start();
    FEPOH_LOG_DEBUG(s_log_system) << "main end";
}
