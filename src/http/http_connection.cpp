#include "http_connection.h"
#include "socket_stream.h"
#include "http_parser.h"
#include "log/log.h"


namespace fepoh{
namespace http{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

std::string HttpResult::tostring(){
    std::stringstream ss;
    ss << "[result = " << result
       << ",error = " << error
       << ",response = " << (response ? response->tostring() : "nullptr")
       << "]";
    return ss.str();
}

std::atomic<uint64_t> HttpConnection::s_count = {0};

HttpConnection::HttpConnection(Socket::ptr sock,bool owner)
    :SocketStream(sock,owner){
    ++s_count;
}

HttpConnection::~HttpConnection(){
    --s_count;
}

HttpResponse::ptr HttpConnection::recvResponse(){
    int max_head_size = HttpResponseParser::GetResponseHeadBufSize();
    int max_size = HttpRequestParser::GetRequestBodyBufSize() + max_head_size;
    HttpResponseParser::ptr parser(new HttpResponseParser());
    std::shared_ptr<char> buffer(new char[max_head_size],[](char* ptr){
        delete[] ptr;
    });
    char* data = buffer.get();
    int offset = 0;
    int total_length = 0;
    do{
        int len = read(data + offset,max_head_size - offset);
        if(len <= 0 ){
            close();
            return nullptr;
        }
        //总长度
        total_length += len;
        //接受到的长度 + 未解析完的长度
        len += offset;
        //执行解析
        size_t nparser = parser->execute(data,len);
        if(parser->getError()){
            if(parser->getError() & HPE_INVALID_CONSTANT){
                FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
                            << ",max size" << max_size; 
            }
            close();
            return nullptr;
        }
        offset = len - nparser;
        if(offset == max_head_size){
            FEPOH_LOG_ERROR(s_log_system) << "offset = max_size";
            close();
            return nullptr;
        }

        if((!parser->getHeadFinish())&&(total_length > max_head_size * 2)){
            FEPOH_LOG_ERROR(s_log_system) << "totol length = " << total_length 
                            << ",max head size" << max_head_size; 
            close();
            return nullptr;
        }
        if(parser->isFinished()){
            break;
        }
    }while(true);
    return parser->getData();
}

HttpConnectionPool::~HttpConnectionPool(){
    for(auto conn : m_conns){
        if(conn){
            delete conn;
            conn = nullptr;
        }
    }
}

int HttpConnection::sendRequest(HttpRequest::ptr request){
    std::stringstream ss;
    request->dump(ss);
    std::string str = ss.str();
    return writeFixSize(str.c_str(),str.size());
}

HttpResult::ptr HttpConnection::DoGet(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    Uri::ptr uri = Uri::CreateUri(url);
    return HttpConnection::DoGet(uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnection::DoGet(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    return HttpConnection::DoRequest(HttpMethod::HTTP_GET,uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnection::DoPost(const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    Uri::ptr uri = Uri::CreateUri(url);
    return HttpConnection::DoPost(uri,timeout_ms,headers,body);
}
    
HttpResult::ptr HttpConnection::DoPost(Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    return HttpConnection::DoRequest(HttpMethod::HTTP_POST,uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                ,const std::string& url
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    Uri::ptr uri = Uri::CreateUri(url);
    return DoRequest(method,uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                ,Uri::ptr uri
                ,uint64_t timeout_ms
                ,const std::map<std::string,std::string>& headers
                ,const std::string& body){
    if(!uri){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_URL,nullptr,"invalid url"));
    }
    HttpRequest::ptr request = uri->creatHttpRequest(method);
    bool has_host = false;
    for(auto& i:headers){
        if(strcasecmp(i.first.c_str(),"Connection") == 0){
            if(strcasecmp(i.second.c_str(),"keep-alive") == 0){
                request->setClose(false);
            }
        }
        if(strcasecmp(i.first.c_str(),"host") == 0){
            uri->setHost(i.second);
            has_host = true;
        }
        request->setHeader(i.first,i.second);
    }
    if(!has_host){
        request->setHeader("Host",uri->getHost());
    }
    request->setBody(body);
    return DoRequest(request,uri,timeout_ms);
}

HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr request
                ,Uri::ptr uri
                ,uint64_t timeout_ms){
    
    Address::ptr addr = uri->createAddr();
    if(!addr){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_ADDR,nullptr,"invalid address"));
    }
    Socket::ptr socket = Socket::CreateTCP(addr);
    if(!socket){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_Socket,nullptr,"invalid socket"));
    }
    if(!socket->connect(addr)){
        socket->close();
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_Socket,nullptr,"socket connect fail"));
    }
    socket->setRcvTimeout(timeout_ms);
   
    HttpConnection::ptr conn(new HttpConnection(socket));
    int rt = conn->sendRequest(request);
    if(rt == 0){
        return HttpResult::ptr(new HttpResult(
            (int)HttpResult::Result::HTTP_SEND_CLOSE_BY_PEER,nullptr,"send close by peer"));
    }else if(rt < 0){
        std::stringstream ss;
        ss << "request error.errno = " << errno << ":" << strerror(errno);
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_SEND_REQUEST_ERROR,nullptr,ss.str()));
    }
    auto response = conn->recvResponse();
    if(!response){
        std::stringstream ss;
        ss << "recv time out.[address:" << addr->tostring() 
                    << "],timeout = " << timeout_ms; 
        return HttpResult::ptr(new HttpResult((int)HttpResult::Result::HTTP_RECV_TIME_OUT,nullptr,ss.str()));
    }
    return HttpResult::ptr(new HttpResult((int)HttpResult::Result::HTTP_SUCCESS,response,"success"));
}


HttpConnectionPool::HttpConnectionPool(const std::string& host,const std::string& m_vhost
        ,uint32_t port,uint32_t max_size,uint64_t max_alive_time,uint32_t max_request)
    :m_host(host),m_vhost(host),m_port(port),m_maxSize(max_size),m_maxAliveTime(max_alive_time)
    ,m_maxRequest(max_request){
    
}

HttpResult::ptr HttpConnectionPool::doGet(const std::string& url
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers
            ,const std::string& body){
    Uri::ptr uri = Uri::CreateUri(url);
    return doGet(uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnectionPool::doGet(Uri::ptr uri
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers  
            ,const std::string& body){
    return doRequest(http::HttpMethod::HTTP_GET,uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnectionPool::doPost(const std::string& url
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers  
            ,const std::string& body  ){
    Uri::ptr uri = Uri::CreateUri(url);
    return doPost(uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnectionPool::doPost(Uri::ptr uri
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers  
            ,const std::string& body){
    return doRequest(http::HttpMethod::HTTP_POST,uri,timeout_ms,headers,body);
}

//请求方法,资源链接,超时时间
HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method
            ,const std::string& url
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers  
            ,const std::string& body){
    Uri::ptr uri = Uri::CreateUri(url);
    return doRequest(method,uri,timeout_ms,headers,body);
}

HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method
            ,Uri::ptr uri
            ,uint64_t timeout_ms
            ,const std::map<std::string,std::string>& headers  
            ,const std::string& body){
    if(!uri){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_URL,nullptr,"invalid url"));
    }
    HttpRequest::ptr request = uri->creatHttpRequest(method);
    bool has_host = false;
    request->setClose(false);
    for(auto& i:headers){
        if(strcasecmp(i.first.c_str(),"host") == 0){
            has_host = true;
        }
        request->setHeader(i.first,i.second);
    }

    if(!has_host){
        if(m_vhost.empty()){
            request->setHeader("Host",m_host);
        }else{
            request->setHeader("Host",m_vhost);
        }
    }
    request->setBody(body);
    return doRequest(request,timeout_ms);
}

HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr request
            ,uint64_t timeout_ms){
    auto conn = getConnection();
    if(!conn){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_GET_CONN_FAIL,nullptr,"get connection fail"));
    }
    Socket::ptr socket = conn->getSocket();
    if(!socket){
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_INVALID_CONN,nullptr,"invalid connection"));
    }
    socket->setRcvTimeout(timeout_ms);
    int rt = conn->sendRequest(request);
    if(rt == 0){
        return HttpResult::ptr(new HttpResult(
            (int)HttpResult::Result::HTTP_SEND_CLOSE_BY_PEER,nullptr,"send close by peer"));
    }else if(rt < 0){
        std::stringstream ss;
        ss << "request error.errno = " << errno << ":" << strerror(errno);
        return HttpResult::ptr(new HttpResult(
                (int)HttpResult::Result::HTTP_SEND_REQUEST_ERROR,nullptr,ss.str()));
    }
    auto response = conn->recvResponse();
    if(!response){
        std::stringstream ss;
        ss << "recv time out.timeout = " << timeout_ms; 
        return HttpResult::ptr(new HttpResult((int)HttpResult::Result::HTTP_RECV_TIME_OUT,nullptr,ss.str()));
    }
    return HttpResult::ptr(new HttpResult((int)HttpResult::Result::HTTP_SUCCESS,response,"success"));
}

HttpConnection::ptr HttpConnectionPool::getConnection(){
    std::vector<HttpConnection*> invalid_conn;
    auto now_ms = fepoh::GetCurTimeMs();
    HttpConnection* ptr = nullptr;
    MutexLock lock(m_mutex);
    while(!m_conns.empty()){
        auto conn = *m_conns.begin();
        if(!conn){
            break;
        }
        if(!conn->isConnected()){
            invalid_conn.push_back(conn);
            m_conns.erase(m_conns.begin());
            continue;
        }
        if(conn->m_createTime + m_maxAliveTime < now_ms){
            invalid_conn.push_back(conn);
            m_conns.erase(m_conns.begin());
            continue;
        }
        if(conn->m_using){
            m_conns.erase(m_conns.begin());
            continue;
        }
        ptr = conn;
        break;
    }
    lock.unlock();
    for(auto& i : invalid_conn){
        delete i;
        i = nullptr;
    }
    m_totalSize -= invalid_conn.size();
    if(!ptr){
        IPAddress::ptr addr = IPAddress::Lookup(m_host,0);
        if(!addr){
            FEPOH_LOG_ERROR(s_log_system) << "Lookup addr error.";
            return nullptr;
        }
        addr->setPort(m_port);
        Socket::ptr socket = Socket::CreateTCP(addr);
        if(!socket){
            FEPOH_LOG_ERROR(s_log_system) << "Create socket error.";
            return nullptr;
        }
        bool rt = socket->connect(addr);
        if(!rt){
            FEPOH_LOG_ERROR(s_log_system) << "socket connect error.";
            return nullptr;
        }
        ptr = new HttpConnection(socket);
        ptr->m_createTime = now_ms;
        ++m_totalSize;
    }else{
        ptr->m_using = true;
    }
    return HttpConnection::ptr(ptr,std::bind(&HttpConnectionPool::ReleasePtr,std::placeholders::_1,this));
}

void HttpConnectionPool::ReleasePtr(HttpConnection* ptr,HttpConnectionPool* pool){
    ++ptr->m_request;
    if(!ptr->isConnected() || (ptr->m_createTime + pool->m_maxAliveTime < fepoh::GetCurTimeMs())
        || ptr->m_request >= pool->m_maxRequest){
        MutexLock lock(pool->m_mutex);
        for(auto it = pool->m_conns.begin();it != pool->m_conns.end();++it){
            if(*it == ptr){
                pool->m_conns.erase(it);
                break;
            }
        }
        delete ptr;
        ptr = nullptr;
        --pool->m_totalSize;
        return ;
    }
    MutexLock lock(pool->m_mutex);
    for(auto conn:pool->m_conns){
        if(conn == ptr){
            return;
        }
    }
    pool->m_conns.push_back(ptr);
    ptr->m_using = false;
}
}
}//namespace

