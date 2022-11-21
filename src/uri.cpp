/*
 * @Author: fepo_h
 * @Date: 2022-11-20 21:31:32
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 15:00:57
 * @FilePath: /fepoh/workspace/fepoh_server/src/uri.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "uri.h"
#include "log/log.h"
#include "http/http_parser/http_nodejs_parser.h"


namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

Uri::ptr Uri::CreateUri(const std::string& uri){
    Uri::ptr rt(new Uri(uri));
    if(rt->parserUri()){
        return rt;
    }
    return nullptr;
}

Uri::Uri(const std::string& uri):m_uri(uri){
}

const std::string& Uri::getPath() const{
    static const std::string default_path = "/";
    if(m_path.empty()){
        return default_path;
    }
    return m_path;
} 


Address::ptr Uri::createAddr(){
    if(m_host.empty()){
        return nullptr;
    }
    return Address::LookupIPAddr(m_host,getPort());
}

http::HttpRequest::ptr Uri::creatHttpRequest(http::HttpMethod method
        ,uint8_t version,bool close){
    if(!m_isInit){
        return nullptr;
    }
    http::HttpRequest::ptr request(new http::HttpRequest(version,close));
    request->setFragment(m_fragment);
    request->setMethod(method);
    request->setPath(getPath());
    request->setVersion(version);
    request->setQuery(m_query);
    return request;
}

bool Uri::modifyHttpRequest(http::HttpRequest::ptr request){
    if(!m_isInit){
        return false;
    }
    request->setFragment(m_fragment);
    request->setPath(getPath());
    request->setQuery(m_query);
    return true;
}


std::ostream& Uri::dump(std::ostream& os){
    os << m_schema << "://"
       << m_userinfo << (m_userinfo.empty() ? "" : "@")
       << m_host << (getPort() ? ":" + std::to_string(getPort()) : "")
       << m_path
       << (m_query.empty() ? "" : "?") << m_query
       << (m_fragment.empty() ? "" : "#") << m_fragment;
    return os;
}

std::string Uri::tostring(){
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

bool Uri::parserUri(){
    struct http_parser_url url_parser;
    http_parser_url_init(&url_parser);
    const char* buf = m_uri.c_str();
    int len = m_uri.size();
    int rt = http_parser_parse_url(buf, len, 0, &url_parser);
    if(rt != 0){
        FEPOH_LOG_ERROR(s_log_system) << "uri parse error";
        return false;
    }
    if (url_parser.field_set & (1 << UF_SCHEMA)) {
        m_schema = std::string(buf + url_parser.field_data[UF_SCHEMA].off,url_parser.field_data[UF_SCHEMA].len);
    }
    if (url_parser.field_set & (1 << UF_USERINFO)) {
        m_userinfo = std::string(buf + url_parser.field_data[UF_USERINFO].off,url_parser.field_data[UF_USERINFO].len);
    }
    if (url_parser.field_set & (1 << UF_HOST)) {
        m_host = std::string(buf + url_parser.field_data[UF_HOST].off,url_parser.field_data[UF_HOST].len);
    }
    if (url_parser.field_set & (1 << UF_PORT)) {
        std::string tmp = std::string(buf + url_parser.field_data[UF_PORT].off,url_parser.field_data[UF_PORT].len);
        m_port = atoi(tmp.c_str());
    }
    if (url_parser.field_set & (1 << UF_PATH)) {
        m_path = std::string(buf + url_parser.field_data[UF_PATH].off,url_parser.field_data[UF_PATH].len);
    }
    if (url_parser.field_set & (1 << UF_QUERY)) {
        m_query = std::string(buf + url_parser.field_data[UF_QUERY].off,url_parser.field_data[UF_QUERY].len);
    }
    if (url_parser.field_set & (1 << UF_FRAGMENT)) {
        m_fragment = std::string(buf + url_parser.field_data[UF_FRAGMENT].off,url_parser.field_data[UF_FRAGMENT].len);
    }
    m_isInit = true;
    return true;
}

uint32_t Uri::getPort(){
    if(m_port != 0){
        return m_port;
    }
    if(m_schema == "http"){
        return 80;
    }
    if(m_schema == "https"){
        return 443;
    }
    if(m_schema == "ftp"){
        return 21;
    }
    if(m_schema == "telnet"){
        return 23;
    }
    if(m_schema == "smtp"){
        return 25;
    }
    return 0;
}

std::string Uri::toUrl(){
    std::stringstream ss;
    ss << getPath()
       << (m_query.empty() ? "" : "?" )
       << m_query
       << (m_fragment.empty() ? "" : "#" )
       << m_fragment;
    return ss.str();
}

}