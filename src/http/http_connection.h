#pragma once

#include "http.h"
#include "socket_stream.h"
#include "uri.h"

namespace fepoh{

namespace http{

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
    HttpResult(int _result,HttpResponse::ptr _response
                                ,const std::string& _error)
              :result(_result),response(_response),error(_error){}

    std::string tostring();
    int result;
    HttpResponse::ptr response;
    std::string error;
};

class HttpConnectionPool;
class HttpConnection : public SocketStream{
public:
    friend class HttpConnectionPool;
    typedef std::shared_ptr<HttpConnection> ptr;

    static HttpResult::ptr DoGet(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

    static HttpResult::ptr DoGet(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers = {}
                ,const std::string& body = "");

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

    HttpResponse::ptr recvResponse();
    int sendRequest(HttpRequest::ptr request);
    static std::atomic<uint64_t> m_count;
private:
    uint64_t m_createTime = 0;
    u_int64_t m_request = 0;
    bool m_using = false;
};

class HttpConnectionPool{
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;

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