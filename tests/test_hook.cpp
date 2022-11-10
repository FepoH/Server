#include "hook.h"
#include "log/log.h"
#include "io_manager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace fepoh;

static fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("system");


void test_hook(){
    IOManager iom;
    iom.schedule([](){
        sleep(2);
        FEPOH_LOG_DEBUG(s_log_system)<<"sleep2";
    });
    iom.schedule([](){
        sleep(3);
        FEPOH_LOG_DEBUG(s_log_system)<<"sleep3";
    });
    iom.start();
    
    iom.stop();
}

void test_io() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "110.242.68.66", &addr.sin_addr.s_addr);

    FEPOH_LOG_INFO(s_log_system) << "begin connect";
    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    FEPOH_LOG_INFO(s_log_system) << "connect rt=" << rt << " errno=" << errno;

    if(rt) {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    FEPOH_LOG_INFO(s_log_system) << "send rt=" << rt << " errno=" << errno;

    if(rt <= 0) {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    FEPOH_LOG_INFO(s_log_system) << "recv rt=" << rt << " errno=" << errno;

    if(rt <= 0) {
        return;
    }

    buff.resize(rt);
    FEPOH_LOG_INFO(s_log_system) << buff;
}


int main(){
    
    //test_hook();
    fepoh::IOManager iom;
    iom.schedule(test_io);
    iom.start();
    //test_io();
    return 0;
}