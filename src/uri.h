#pragma once


#include "address.h"
#include "http/http.h"
#include "thread/mutex.h"
#include <memory>
#include <string>
#include <list>
#include <atomic>

namespace fepoh{

class Uri{
public:
    typedef std::shared_ptr<Uri> ptr;

    static Uri::ptr CreateUri(const std::string& uri);

    http::HttpRequest::ptr creatHttpRequest(http::HttpMethod method
                                    ,uint8_t version = 0x11,bool close = true);

    void modifyHttpRequest(http::HttpRequest::ptr request);

    Uri(const std::string& uri);


    const std::string& getSchema() const {return m_schema;}
    const std::string& getUserinfo() const {return m_userinfo;}
    const std::string& getHost() const {return m_host;}
    const std::string& getPath() const;
    const std::string& getQuery() const {return m_query;}
    const std::string& getFragment() const {return m_fragment;}

    void setSchema(const std::string& v) {m_schema = v;}
    void setUserinfo(const std::string& v) {m_userinfo = v;}
    void setHost(const std::string& v) {m_host = v;}
    void setPort(uint32_t v) {m_port = v;} 
    void setPath(const std::string& v) {m_path = v;}
    void setQuery(const std::string& v) {m_query = v;}
    void setFragment(const std::string& v) {m_fragment = v;}

    Address::ptr createAddr();

    std::ostream& dump(std::ostream& os);
    std::ostream& dump1(std::ostream& os);
    std::string tostring();
    std::string toUrl();

    uint32_t getPort();

    void parserUri();
private:
    std::string m_uri;               //原始uri
    std::string m_schema = "";       //协议
    std::string m_userinfo = "";     //用户信息
    std::string m_host = "";         //host
    uint32_t    m_port = 0;          //端口
    std::string m_path = "";         //路径
    std::string m_query = "";        //query
    std::string m_fragment = "";     //fragment
};


}