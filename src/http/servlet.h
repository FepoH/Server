/*
 * @Author: fepo_h
 * @Date: 2022-11-17 23:01:19
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 14:38:26
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/servlet.h
 * @Description: 服务连接器
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include "http.h"
#include "http_session.h"
#include "thread/mutex.h"

namespace fepoh{
namespace http{

/**
 * @description: 连接器基类
 */
class Servlet{
public:
    typedef std::shared_ptr<Servlet> ptr;

    Servlet(const std::string& name):m_name(name){}
    virtual ~Servlet(){}
    /**
     * @description: 处理函数
     * @return {*}
     */    
    virtual int32_t handle(HttpRequest::ptr request
                            ,HttpResponse::ptr response
                            ,HttpSession::ptr session) = 0;
    
    const std::string& getName() const {return m_name;}
protected:
    std::string m_name;
};

/**
 * @description: 回调连接器
 */
class FunctionServlet : public Servlet{
public:
    typedef  std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t(HttpRequest::ptr request
                            ,HttpResponse::ptr response
                            ,HttpSession::ptr session)> callbck;
    
    FunctionServlet(callbck cb);
    /**
     * @description: 处理函数
     * @return {*}
     */    
    int32_t handle(HttpRequest::ptr request
            ,HttpResponse::ptr rsponse,HttpSession::ptr session) override;
private:
    //回调函数
    callbck m_cb;
};

//404 Not Found连接器
class NotFoundServlet : public Servlet{
public:
    typedef  std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet():Servlet("NotFoundServlet"){}

    int32_t handle(HttpRequest::ptr request
            ,HttpResponse::ptr rsponse,HttpSession::ptr session) override;
};

/**
 * @description: servlet 管理类
 */
class ServletManager : public Servlet{
public:
    typedef std::shared_ptr<ServletManager> ptr;

    typedef std::function<int32_t(HttpRequest::ptr request
                        ,HttpResponse::ptr response
                        ,HttpSession::ptr session)> callbck;
    //构造函数
    ServletManager();

    int32_t handle(HttpRequest::ptr request,HttpResponse::ptr response
                            ,HttpSession::ptr session);
    /**
     * @description: 添加servlet
     * @return {*}
     * @param {string&} uri
     * @param {ptr} slt
     */    
    void addServlet(const std::string& uri,Servlet::ptr slt);
    /**
     * @description: 添加servlet
     * @return {*}
     * @param {string&} uri
     * @param {callbck} cb
     */    
    void addServlet(const std::string& uri,FunctionServlet::callbck cb);
    /**
     * @description: 添加serlet:模糊匹配
     * @return {*}
     * @param {string&} uri
     * @param {ptr} slt
     */    
    void addGlobServlet(const std::string& uri,Servlet::ptr slt);
    /**
     * @description: 添加serlet:模糊匹配
     * @return {*}
     * @param {string&} uri
     * @param {callbck} cb
     */    
    void addGlobServlet(const std::string& uri,FunctionServlet::callbck cb);
    /**
     * @description: 删除serlet
     * @return {*}
     * @param {string&} uri
     */    
    void delServlet(const std::string& uri);
    /**
     * @description: 删除sevlet
     * @return {*}
     * @param {string&} uri
     */    
    void delGlobServlet(const std::string& uri);

    /**
     * @description: 设置默认servlet
     * @return {*}
     * @param {ptr} v
     */    
    void setDefault(Servlet::ptr v) {m_default = v;}
    /**
     * @description: 获取默认servlet
     * @return {*}
     */    
    Servlet::ptr getDefault() {return m_default;}

    /**
     * @description: 获取servlet
     * @return {*}
     * @param {string&} uri
     */    
    Servlet::ptr getServlet(const std::string& uri);
    /**
     * @description: 获取sevlet:模糊匹配
     * @return {*}
     * @param {string&} uri
     */    
    Servlet::ptr getGlobServlet(const std::string& uri);
    /**
     * @description: 获取sevlet,先通过精准匹配,找不到再通过模糊匹配
     * @return {*}
     * @param {string&} uri
     */    
    Servlet::ptr getMatchServlet(const std::string& uri);
private:
    RWMutex m_mutex;    //锁
    Servlet::ptr m_default;     //默认servlet
    std::unordered_map<std::string,Servlet::ptr> m_data;    //精准匹配的servlet
    std::vector<std::pair<std::string,Servlet::ptr>> m_globData;    //模糊匹配的servlet
    
};

}


}