#include "socket_.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_client(int port){
    Address::ptr addr = IPAddress::Lookup("192.168.159.147",port);
    if(!addr){
        FEPOH_LOG_ERROR(s_log_system) << "Look up error";
    }else{
        FEPOH_LOG_INFO(s_log_system) << addr->tostring();
    }

    Socket::ptr sock = Socket::CreateTCP(addr);
    sock->connect(addr);
    char buf[1024];
    int client = 0;
    while(1){
        bzero(buf,1024);
        int n = sock->recv(buf,1024);
        if(n <= 0){
            FEPOH_LOG_ERROR(s_log_system) << "recv error n = " << n;
        }else{
            FEPOH_LOG_INFO(s_log_system) << sock->tostring();
            FEPOH_LOG_INFO(s_log_system) << buf;
        }
        bzero(buf,1024);
        sprintf(buf,"%s = %d","client",client);
        n = sock->send(buf,strlen(buf));
        sleep(3);
        ++client;
    }

}


int main(int argc,char*argv[]){
    if(argc != 2){
        return -1;
    }
    int port = atoi(argv[1]);
    test_client(port);
}