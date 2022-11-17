#pragma once


#include "http.h"
#include "socket_stream.h"

namespace fepoh{

namespace http{

class HttpSession : public SocketStream{
public:
    typedef std::shared_ptr<HttpSession> ptr;

    HttpSession(Socket::ptr sock,bool owner = true);

    HttpRequest::ptr recvRequrest();
    int sendResponse(HttpResponse::ptr response);


};

}
}//namespace