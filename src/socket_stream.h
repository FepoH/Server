#pragma once

#include "stream.h"
#include "socket_.h"

namespace fepoh{

class SocketStream : public Stream{
public:
    typedef std::shared_ptr<SocketStream> ptr;

    SocketStream(Socket::ptr sock,bool owner = true);
    ~SocketStream();

    Socket::ptr getSocket(){return m_socket;}
    int read(void* buffer,size_t length) override;
    int read(ByteArray::ptr ba,size_t length) override;
    int write(const void* buffer,size_t length) override;
    int write(ByteArray::ptr ba,size_t length) override;
    bool isConnected();
    void close() override;
private:
    Socket::ptr m_socket;
    bool m_owner;
};


}//namespace