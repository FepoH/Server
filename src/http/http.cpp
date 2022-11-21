/*
 * @Author: fepo_h
 * @Date: 2022-11-20 22:20:44
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 15:02:05
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "http.h"

namespace fepoh{

namespace http{

static const char* s_http_method_map[] = {
#define XX(num,name,string) #name,
    HTTP_METHOD_MAP(XX)
#undef XX
};

static const char* s_http_status_map[] = {
#define XX(num,name,string) #name,
    HTTP_STATUS_MAP(XX)
#undef XX
};

HttpMethod StringToHttpMethod(const std::string& str){
    return CharsToHttpMethod(str.c_str());
}
HttpMethod CharsToHttpMethod(const char* str){
#define XX(num,name,string) \
    if(strcasecmp(str,#string) == 0){        \
        return (HttpMethod)num;         \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::HTTP_METHOD_INVALID;
}

const char* HttpMethodToChars(HttpMethod hm){
    if((int)hm >= sizeof(s_http_method_map) / sizeof(s_http_method_map[0])){
        return "<unkown>";
    }
    return s_http_method_map[(int)hm];
}
const char* HttpStatusToChars(HttpStatus hs){
    switch((int)hs){
#define XX(num,name,string) \
        case num:           \
            return #string;   \
            break;
        HTTP_STATUS_MAP(XX);
#undef XX
        default:
            return "<unkown>";
    }
    return "<unkown>";
}


bool CaseInsensitiveLess::operator()(const std::string& lhs,const std::string& rhs){
    return strcasecmp(lhs.c_str(),rhs.c_str()) < 0;
}


HttpRequest::HttpRequest(uint8_t version,bool close)
        :m_version(version),m_close(close),m_method(HttpMethod::HTTP_GET){
}

std::ostream& HttpRequest::dump(std::ostream& os){
    os << HttpMethodToChars(m_method)  << " "
       << (m_path.empty() ? "/" : m_path) << (m_query.empty() ? "" : "?")
       << m_query
       << (m_fragment.empty() ? "" : "#")
       << m_fragment << " "
       << "HTTP/" <<(uint32_t)(m_version >> 4) << "."
       <<(uint32_t)(m_version & 0x0F) << "\r\n";
    os << "Connection:" << (m_close ? "close" : "keep-alive") << "\r\n";
    for(auto& item:m_headers){
        if((strcasecmp(item.first.c_str(),"Connection") == 0)
            ||(strcasecmp(item.first.c_str(),"Content-Length") == 0)){
            continue;
        }
        os << item.first << ":" << item.second << "\r\n"; 
    }
    if(!m_body.empty()){
        os << "Content-Length:" << m_body.size() << "\r\n\r\n" <<m_body;
    }else{
        os << "\r\n";
    }
    return os;
}

std::string HttpRequest::tostring(){
    std::stringstream ss;
    dump(ss);
    return ss.str();
}


bool HttpRequest::has(MapType type,const std::string& key) const{
    auto it = type.find(key);
    if(it != type.end()){
        return true;
    }
    return false;
}


//set
void HttpRequest::setHeader(const std::string& key,const std::string& value){
    m_headers[key] = value;
}
void HttpRequest::setPara(const std::string& key,const std::string& value){
    m_paras[key] = value;
}
void HttpRequest::setCookie(const std::string& key,const std::string& value){
    m_cookies[key] = value;
}
//has
bool HttpRequest::hasHeader(const std::string& key){
    return has(m_headers,key);
}
bool HttpRequest::hasPara(const std::string& key){
    return has(m_paras,key);
}
bool HttpRequest::hasCookie(const std::string& key){
    return has(m_cookies,key);
}
//del
void HttpRequest::delHeader(const std::string& key){
    m_headers.erase(key);
}
void HttpRequest::delPara(const std::string& key){
    m_paras.erase(key);
}
void HttpRequest::delCookie(const std::string& key){
    m_cookies.erase(key);
}

uint64_t HttpRequest::getContentLength(){
    return getHeaderAs<uint64_t>("Content-Length",0);
}



HttpResponse::HttpResponse(uint8_t m_version,bool close):m_version(m_version)
        ,m_close(close),m_status(HttpStatus::HTTP_OK){
}


bool HttpResponse::has(MapType type,const std::string& key) const{
    auto it = type.find(key);
    if(it != type.end()){
        return true;
    }
    return false;
}

void HttpResponse::setStatus(HttpStatus v){
    m_status = v;
    m_reason = HttpStatusToChars(m_status);
}

//set
void HttpResponse::setHeader(const std::string& key,const std::string& value){
    m_headers[key] = value;
}

void HttpResponse::setCookie(const std::string& key,const std::string& value){
    m_cookies[key] = value;
}
//has
bool HttpResponse::hasHeader(const std::string& key){
    return has(m_headers,key);
}

bool HttpResponse::hasCookie(const std::string& key){
    return has(m_cookies,key);
}
//del
void HttpResponse::delHeader(const std::string& key){
    m_headers.erase(key);
}

void HttpResponse::delCookie(const std::string& key){
    m_cookies.erase(key);
}

uint64_t HttpResponse::getContentLength(){
    return getHeaderAs<uint64_t>("Content-Length",0);
}

std::ostream& HttpResponse::dump(std::ostream& os){
    os << "HTTP/" << ((uint32_t)(m_version >> 4)) << "." 
       << ((uint32_t)(m_version & 0x0F)) << " " << (int)m_status << " " 
       << (m_reason.empty() ? HttpStatusToChars(m_status) : m_reason) << "\r\n";
    os << "Connection:" << (m_close ? "close" : "keep-alive") << "\r\n";
    for(auto& item:m_headers){
        if((strcasecmp(item.first.c_str(),"Connection") == 0)
            ||(strcasecmp(item.first.c_str(),"Content-Length") == 0)){
            continue;
        }
        os << item.first << ":" << item.second << "\r\n"; 
    }
    if(!m_body.empty()){
        os << "Content-Length:" << m_body.size() << "\r\n\r\n" <<m_body ;
    }else{
        os << "\r\n";
    }
    return os;
}

std::string HttpResponse::tostring(){
    std::stringstream ss;
    dump(ss);
    return ss.str();
}


}

}

