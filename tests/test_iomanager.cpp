#include "io_manager.h"
#include "log/log.h"
#include "timer.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");


void test_func1();
static IOManager::ptr iom(new IOManager("iom",1,true));
static Timer::ptr t(new Timer(test_func1,2000,iom.get(),true));

void test_func(){
    FEPOH_LOG_DEBUG(s_log_system) <<" test**********";
}

void test_func1(){
    static int i = 0;
    ++i;
    FEPOH_LOG_DEBUG(s_log_system) <<" test timer**********";
    if(i == 1){
        t->setRecurring(false);
    }
}

void test(){
    FEPOH_LOG_DEBUG(s_log_system) <<"main start";
    iom->addTimer(t);
    for(int i=0;i<4;++i){
        iom->schedule(test_func);
    }
    iom->start();
    iom->stop();
}

int sock = 0;

void test_epoll(){
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,"110.242.68.66",&addr.sin_addr);
    addr.sin_port = htons(80);
    sock = socket(AF_INET,SOCK_STREAM,0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
        std::cout << 1 << std::endl;
    } else if(errno == EINPROGRESS) {
        FEPOH_LOG_DEBUG(s_log_system) << "add event errno=" << errno << " " << strerror(errno);
        IOManager::GetThis()->addEvent(sock, IOManager::READ, [](){
            FEPOH_LOG_INFO(s_log_system) << "read callback***********";
        });
        IOManager::GetThis()->addEvent(sock, IOManager::WRITE, [](){
            FEPOH_LOG_INFO(s_log_system) << "write callback**********";
            //close(sock);
            IOManager::GetThis()->cancelEvent(sock,  IOManager::READ);
            close(sock);
        });
    } else {
        FEPOH_LOG_DEBUG(s_log_system) << "else " << errno << " " << strerror(errno);
    }
}

void test1() {
    IOManager::ptr iom1(new IOManager("fepoh",1, true));

    iom1->schedule(test_epoll);
    iom1->start();
}

int main(){
    test();
    test1();
}