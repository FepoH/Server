/*
 * @Author: fepo_h
 * @Date: 2022-11-18 19:40:49
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 21:20:55
 * @FilePath: /fepoh/workspace/fepoh_server/src/uri.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "address.h"
#include "http/http.h"
#include "thread/mutex.h"

#include <memory>
#include <string>
#include <list>
#include <atomic>

namespace fepoh{

/**
 * @description: uri解析类
 */
class Uri{
public:
    typedef std::shared_ptr<Uri> ptr;
    /**
     * @description: 根据uri创建Uri对象
     * @return {*}
     * @param {string&} uri
     */    
    static Uri::ptr CreateUri(const std::string& uri);
    /**
     * @description: 通过Uri数据创建HttpRequest
     * @return {*}
     */    
    http::HttpRequest::ptr creatHttpRequest(http::HttpMethod method
                                    ,uint8_t version = 0x11,bool close = true);
    /**
     * @description: 通过Uri数据修改HttpRequest
     * @return {*}
     * @param {ptr} request
     */    
    bool modifyHttpRequest(http::HttpRequest::ptr request);
    /**
     * @description: 构造函数
     * @return {*}
     * @param {string&} uri
     */    
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
    /**
     * @description: 创建Address::ptr 
     * @return {*}
     */    
    Address::ptr createAddr();
    /**
     * @description: dump
     * @return {*}
     * @param {ostream&} os
     */    
    std::ostream& dump(std::ostream& os);
    std::string tostring();
    /**
     * @description: 生成url
     * @return {*}
     */    
    std::string toUrl();
    /**
     * @description: 获取端口,若没有设置,则根据schema生成
     * @return {*}
     */    
    uint32_t getPort();
    /**
     * @description: 转换操作
     * @return {*}
     */    
    bool parserUri();
private:
    bool m_isInit = false;           
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

