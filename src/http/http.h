/*
 * @Author: fepo_h
 * @Date: 2022-11-17 23:13:49
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 22:06:42
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http.h
 * @Description: http请求和http响应封装
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace fepoh {
namespace http {

/*
    GET / HTTP/1.1 
*/

/* http 请求方法*/
/* Request Methods */
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* WebDAV */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  /* subversion */                  \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  /* CalDAV */                      \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */  \
  XX(31, LINK,        LINK)         \
  XX(32, UNLINK,      UNLINK)       \
  /* icecast */                     \
  XX(33, SOURCE,      SOURCE)       \

/* http 状态*/
/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

/**
 * @description: http请求枚举类
 * @return {*}
 */
enum class HttpMethod{
#define XX(num,name,string) HTTP_ ## name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    HTTP_METHOD_INVALID
};

/**
 * @description: http响应枚举类
 * @return {*}
 */
enum class HttpStatus {
#define XX(code,name,desc) HTTP_ ## name = code,
    HTTP_STATUS_MAP(XX)
#undef XX
};
/**
 * @description: string转HttpMethod
 * @return {*}
 * @param {string&} str
 */
HttpMethod StringToHttpMethod(const std::string& str);
/**
 * @description: HttpMethod转char*
 * @return {*}
 * @param {char*} str
 */
HttpMethod CharsToHttpMethod(const char* str);
/**
 * @description: HttpMethod转char*
 * @return {*}
 * @param {HttpMethod} hm
 */
const char* HttpMethodToChars(HttpMethod hm);
/**
 * @description: HttpStatus转char*
 * @return {*}
 * @param {HttpStatus} hs
 */
const char* HttpStatusToChars(HttpStatus hs);

/**
 * @description: 字符串比较(忽略大小写)
 * @return {*}
 */
struct CaseInsensitiveLess{
    bool operator()(const std::string& lhs,const std::string& rhs);
};

/**
 * @description: http请求
 * @return {*}
 */
class HttpRequest{
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    typedef std::map<std::string,std::string,CaseInsensitiveLess> MapType;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {uint8_t} version 默认为http/1.1
     * @param {bool} close 默认为短连接
     */    
    HttpRequest(uint8_t version = 0x11,bool close = true);

    //get
    HttpMethod getMethod() const {return m_method;}
    uint8_t getVersion() const {return m_version;}
    bool getClose() const {return m_close;}
    const std::string& getQuery() const {return m_query;}
    const std::string& getPath() const {return m_path;}
    const std::string& getFragment() const {return m_fragment;}
    const std::string& getBody() const {return m_body;}
    MapType getHeaders() const {return m_headers;}
    MapType getParas() const {return m_paras;}
    MapType getCookies() const {return m_cookies;}

    //set
    void setClose(bool v) {m_close = v;}
    void setQuery(const std::string& v) {m_query = v;}
    void setMethod (HttpMethod v) {m_method = v;}
    void setVersion(uint8_t v) {m_version = v;}
    void setPath(const std::string& v) {m_path = v;}
    void setBody(const std::string& v) {m_body = v;}
    void setFragment(const std::string& v) {m_fragment = v;}
    void setHeaders(const MapType& v) {m_headers = v;}
    void setParas(const MapType& v) {m_paras = v;}
    void setCookies(const MapType& v) {m_cookies = v;}
    uint64_t getContentLength();

    /**
     * @description: 获取value并转为对应类型
     * @return {*}
     */    
    template<class T>
    T getHeaderAs(const std::string& key,T def = T()){
        return getAs<T>(m_headers,key,def);
    }
    template<class T>
    T getParaAs(const std::string& key,T def = T()){
        return getAs(m_paras,key,def);
    }
    template<class T>
    T getCookie(const std::string& key,T def = T()){
        return getAs(m_cookies,key,def);
    }
    //set
    void setHeader(const std::string& key,const std::string& value);
    void setPara(const std::string& key,const std::string& value);
    void setCookie(const std::string& key,const std::string& value);
    //has
    bool hasHeader(const std::string& key);
    bool hasPara(const std::string& key);
    bool hasCookie(const std::string& key);
    //del
    void delHeader(const std::string& key);
    void delPara(const std::string& key);
    void delCookie(const std::string& key);
    /**
     * @description: 序列化
     * @return {*}
     * @param {ostream&} os
     */    
    std::ostream& dump(std::ostream& os);
    std::string tostring();
private:
    template<class T>
    T getAs(MapType data,const std::string& key,T def = T()){
        auto it = data.find(key);
        if(it == data.end()){
            try{
                T tmp = boost::lexical_cast<T>(it->second);
                return tmp;
            }catch(...){
                return def;
            }
        }
        return def; 
    }
    bool has(MapType type,const std::string& key) const;

    //uri http://www.baidu.com:80/page/xxx?id=10&v=20#frggment
    //    协议      host      端口   路径    query     fragment
    //    schema    host      port   path   query     fragment

    HttpMethod m_method;        //方法字段
    uint8_t m_version;          //http版本
    bool m_close;               //长连接还是短连接
    std::string m_path;         //路径      
    std::string m_fragment;     //fragment
    std::string m_query;        //query
    std::string m_body;         //body

    MapType m_headers;          //首部   
    MapType m_paras;            //参数
    MapType m_cookies;          //cookies
};


class HttpResponse{
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    typedef std::map<std::string,std::string,CaseInsensitiveLess> MapType;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {uint8_t} m_version 版本号
     * @param {bool} close 短连接还是长连接
     */    
    HttpResponse(uint8_t m_version = 0x11,bool close = true);

    uint8_t getVersion() const {return m_version;}
    bool getClose() const {return m_close;}
    HttpStatus getStatus() const {return m_status;}
    const std::string& getReason() const {return m_reason;}
    const std::string& getBody() const {return m_body;}
    const MapType& getHeaders() const {return m_headers;}
    const MapType& getCookies() const {return m_cookies;}
    uint64_t getContentLength();

    void setVersion(uint8_t v) {m_version = v;}
    void setClose(bool v) {m_close = v;}
    void setStatus(HttpStatus v);
    void setReason(const std::string& reason) {m_reason = reason;}
    void setBody(const std::string& body) {m_body = body;}
    void setHeaders(const MapType& v) {m_headers = v;}
    void setCookies(const MapType& v) {m_cookies = v;}
    //get
    template<class T>
    T getHeaderAs(const std::string& key,T def = T()){
        return getAs<T>(m_headers,key,def);
    }
    template<class T>
    T getCookie(const std::string& key,T def = T()){
        return getAs(m_cookies,key,def);
    }
    //set
    void setHeader(const std::string& key,const std::string& value);
    void setCookie(const std::string& key,const std::string& value);
    //has
    bool hasHeader(const std::string& key);
    bool hasCookie(const std::string& key);
    //has
    void delHeader(const std::string& key);
    void delCookie(const std::string& key);
    /**
     * @description: 序列化
     * @return {*}
     * @param {ostream&} os
     */    
    std::ostream& dump(std::ostream& os);
    std::string tostring();
private:
    template<class T>
    T getAs(MapType data,const std::string& key,T def = T()){
        auto it = data.find(key);
        if(it == data.end()){
            try{
                T tmp = boost::lexical_cast<T>(it->second);
                return tmp;
            }catch(...){
                return def;
            }
        }
        return def; 
    }
    bool has(MapType type,const std::string& key) const;

    uint8_t m_version;      //http版本
    HttpStatus m_status;    //响应码状态
    std::string m_reason;   //状态描述
    std::string m_body;     //body
    bool m_close;           //长连接和短连接

    MapType m_headers;      //首部
    MapType m_cookies;      //
};










}

}