/*
 * @Author: fepo_h
 * @Date: 2022-11-18 12:12:24
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 14:05:42
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http_session.h
 * @Description: http seesion 服务器端
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once


#include "http.h"
#include "socket_stream.h"

namespace fepoh{

namespace http{

class HttpSession : public SocketStream{
public:
    typedef std::shared_ptr<HttpSession> ptr;

    HttpSession(Socket::ptr sock,bool owner = true);
    /**
     * @description: 接受请求
     * @return {*}
     */    
    HttpRequest::ptr recvRequrest();
    /**
     * @description: 发送响应
     * @return {*}
     * @param {ptr} response
     */    
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