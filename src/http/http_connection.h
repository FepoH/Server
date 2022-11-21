/*
 * @Author: fepo_h
 * @Date: 2022-11-19 02:14:32
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 17:56:35
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http_connection.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "http.h"
#include "socket_stream.h"
#include "uri.h"

namespace fepoh{

namespace http{

/**
 * @description: http 结果
 * @return {*}
 */
struct HttpResult{
    enum class Result{
        //成功
        HTTP_SUCCESS = 0,           //成功
        //request error
        HTTP_INVALID_URL,           //非法url
        HTTP_INVALID_ADDR,          //非法addr
        HTTP_INVALID_Socket,        //非法socket
        HTTP_SEND_CLOSE_BY_PEER,    //远端关闭连接
        HTTP_SEND_REQUEST_ERROR,    //发送请求错误
        HTTP_RECV_TIME_OUT,         //超时
        HTTP_SOCKET_CONNECT_FAIL,   //connect fail
        HTTP_GET_CONN_FAIL,         //从连接池中取连接失败
        HTTP_INVALID_CONN,          //非法连接
    };
    typedef std::shared_ptr<HttpResult> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     */    
    HttpResult(int _result,HttpResponse::ptr _response
                                ,const std::string& _error)
              :result(_result),response(_response),error(_error){}
    //序列化
    std::string tostring();
    //result code
    int result;
    //result response
    HttpResponse::ptr response;
    //error描述
    std::string error;
};

class HttpConnectionPool;
/**
 * @description: http connection(客户端)
 * @return {*}
 */
class HttpConnection : public SocketStream{
public:
    friend class HttpConnectionPool;
    typedef std::shared_ptr<HttpConnection> ptr;
    /**
     * @description: 发起GET
     * @return {*}
     * @param url 
     * @param timeout_ms 超时时间
     * @param headers 首部
     * @param body
     */    
    static HttpResult::ptr DoGet(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");
 
    static HttpResult::ptr DoGet(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    /**
     * @description: 发起POST
     * @return {*}
     */    
    static HttpResult::ptr DoPost(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");
    
    static HttpResult::ptr DoPost(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    //请求方法,资源链接,超时时间
    static HttpResult::ptr DoRequest(HttpMethod method
                ,const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpMethod method
                ,Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpRequest::ptr request
                ,Uri::ptr uri
                ,uint64_t timeout_ms);

    HttpConnection(Socket::ptr sock,bool owner = true);

    ~HttpConnection();
    /**
     * @description: 接受响应
     * @return {*}
     */    
    HttpResponse::ptr recvResponse();
    /**
     * @description: 发送请求
     * @return {*}
     * @param {ptr} request
     */    
    int sendRequest(HttpRequest::ptr request);
    static std::atomic<uint64_t> s_count;
private:
    uint64_t m_createTime = 0;
    u_int64_t m_request = 0;
    bool m_using = false;
};

class HttpConnectionPool{
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;


    /**
     * @description: 构造函数
     * @return {*}
     * @param host
     * @param host
     * @param max_size 池的最大容量
     * @param max_alive_time 最大存活时间
     * @param max_request 最大请求数
     */    
    HttpConnectionPool(const std::string& host
                      ,const std::string& m_vhost
                      ,uint32_t port
                      ,uint32_t max_size
                      ,uint64_t max_alive_time
                      ,uint32_t max_request);

    HttpResult::ptr doGet(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    HttpResult::ptr doGet(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    HttpResult::ptr doPost(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");
    
    HttpResult::ptr doPost(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    //请求方法,资源链接,超时时间
    HttpResult::ptr doRequest(HttpMethod method
                ,const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    HttpResult::ptr doRequest(HttpMethod method
                ,Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    HttpResult::ptr doRequest(HttpRequest::ptr request
                ,uint64_t timeout_ms);
    uint64_t getTotalSize() const {return m_totalSize;}
    
private:
    static void ReleasePtr(HttpConnection* ptr,HttpConnectionPool* pool);

    HttpConnection::ptr getConnection();
public:
    std::string m_host;         //host
    std::string m_vhost;        //http/1.1,头部一定要host
    uint32_t m_port;            //端口
    uint32_t m_maxSize;         //最多存放的链接数
    uint32_t m_maxAliveTime;    //最大存活时间
    uint64_t m_maxRequest;      //最大请求数

    Mutex m_mutex;
    std::list<HttpConnection*> m_conns; //连接池
    std::atomic<int32_t> m_totalSize = {0}; //总数
};


}
}//namespace