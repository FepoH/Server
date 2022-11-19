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

        // if(parser->getError() & HPE_INVALID_CONSTANT){
        //     FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
        //                     << ",max size" << max_size; 
        //     return nullptr;
        // }

        // if((!parser->getHeadFinish())&&(total_length > max_head_size * 2)){
        //     FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
        //                     << ",max head size" << max_head_size; 
        //     return nullptr;
        // }