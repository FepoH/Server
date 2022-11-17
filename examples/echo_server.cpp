#include "tcp_server.h"
#include "bytearray.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

class EchoServer : public TcpServer{
public:
    EchoServer(int type);
    void handleClient(Socket::ptr client);
private:
    int m_type = 0;
};


EchoServer::EchoServer(int type):m_type(type){

}

void EchoServer::handleClient(Socket::ptr client){
    FEPOH_LOG_DEBUG(s_log_system) << "handle client";
    ByteArray::ptr ba(new ByteArray());
    while(true){
        // ba->clear();
        // std::vector<iovec> iov;
        // ba->getWriteBuffers(iov,1024);
        std::string buf;
        buf.resize(1024);
        //buf = "";
        int rt = client->recv(&buf[0],1024);
        if(rt == 0){
            FEPOH_LOG_DEBUG(s_log_system) << "client close:" << client->tostring();
            break;
        }else if(rt < 0){
            FEPOH_LOG_DEBUG(s_log_system) << "client error.errno = " << errno << ":" << strerror(errno);
            break;
        }else{
            // ba->setPosition(ba->getPosition() + rt);
            // //ba->setPosition(0);
            // if(this->m_type == 0){
            //     std::cout << ba->toString() << std::flush;
            // }else{
            //     std::cout << ba->toHexString() << std::flush;
            // }
            std::cout << buf << std::flush;
        }
    }
}

int type = 0;

void test(){
    EchoServer::ptr echo(new EchoServer(type));
    Address::ptr addr = Address::LookupIPAddr("0.0.0.0:8033",0);
    while(!echo->bind(addr)){
        sleep(2);
    }
    echo->start();
}



int main(int argc,char* argv[]){
    if(argc < 2){
        std::cout << "Using:" << argv[0] << "-b or -t";
        return 0;
    }
    if(strncmp(argv[1],"-b",2) == 0){
        type = 1;
    }
    IOManager iom;
    iom.schedule(test);
    iom.start();
}