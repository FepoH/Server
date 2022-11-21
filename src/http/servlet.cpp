/*
 * @Author: fepo_h
 * @Date: 2022-11-21 14:47:38
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 15:01:58
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/servlet.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "servlet.h"
#include "log/log.h"
#include <fnmatch.h>


namespace fepoh{
namespace http{


FunctionServlet::FunctionServlet(callbck cb)
    :Servlet("FunctionServlet"),m_cb(cb){
}

int32_t FunctionServlet::handle(HttpRequest::ptr request
        ,HttpResponse::ptr response,HttpSession::ptr session){
    return m_cb(request,response,session);
}


ServletManager::ServletManager():Servlet("ServletManager"){
    m_default.reset(new NotFoundServlet());
}

int32_t ServletManager::handle(HttpRequest::ptr request,HttpResponse::ptr response
                        ,HttpSession::ptr session){
    Servlet::ptr slt = getMatchServlet(request->getPath());
    if(slt){
        return slt->handle(request,response,session);
    }
    m_default->handle(request,response,session);
    return 0;
}

void ServletManager::addServlet(const std::string& uri,Servlet::ptr slt){
    WriteLock lock(m_mutex);
    m_data[uri] = slt;
}
void ServletManager::addServlet(const std::string& uri,FunctionServlet::callbck cb){
    WriteLock lock(m_mutex);
    m_data[uri] = Servlet::ptr(new FunctionServlet(cb));
}
void ServletManager::addGlobServlet(const std::string& uri,Servlet::ptr slt){
    WriteLock lock(m_mutex);
    for(auto it = m_globData.begin();it != m_globData.end();++it){
        if(it->first == uri){
            m_globData.erase(it);
            break;
        }
    }
    m_globData.push_back(std::make_pair(uri,slt));
}
void ServletManager::addGlobServlet(const std::string& uri,FunctionServlet::callbck cb){
    WriteLock lock(m_mutex);
    for(auto it = m_globData.begin();it != m_globData.end();++it){
        if(it->first == uri){
            m_globData.erase(it);
            break;
        }
    }
    Servlet::ptr slt(new FunctionServlet(cb));
    m_globData.push_back(std::make_pair(uri,slt));
}
void ServletManager::delServlet(const std::string& uri){
    WriteLock lock(m_mutex);
    auto it = m_data.find(uri);
    if(it != m_data.end()){
        m_data.erase(it);
    }
}
void ServletManager::delGlobServlet(const std::string& uri){
    WriteLock lock(m_mutex);
    for(auto it = m_globData.begin();it != m_globData.end();++it){
        if(it->first == uri){
            m_globData.erase(it);
            break;
        }
    }
}
Servlet::ptr ServletManager::getServlet(const std::string& uri){
    ReadLock lock(m_mutex);
    auto it = m_data.find(uri);
    if(it != m_data.end()){
        return m_data[uri];
    }
    return nullptr;
}

Servlet::ptr ServletManager::getGlobServlet(const std::string& uri){
    ReadLock lock(m_mutex);
    for(auto it = m_globData.begin();it != m_globData.end();++it){
        if(!fnmatch(it->first.c_str(),uri.c_str(),0)){
            return it->second;
        }
    }
    return nullptr;
}
Servlet::ptr ServletManager::getMatchServlet(const std::string& uri){
    ReadLock lock(m_mutex);
    auto it = m_data.find(uri);
    if(it != m_data.end()){
        return m_data[uri];
    }
    for(auto it1 = m_globData.begin();it1 != m_globData.end();++it1){
        if(!fnmatch(it1->first.c_str(),uri.c_str(),0)){
            return it1->second;
        }
    }
    return nullptr;
}


int32_t NotFoundServlet::handle(HttpRequest::ptr request
        ,HttpResponse::ptr response,HttpSession::ptr session){
    static const std::string not_found_body = "<html><head>"
                                        "<title>404 Not Found</title>"
                                        "</head><body>"
                                        "<h1>Not Found</h1>"
                                        "<hr>"
                                        "<address>fepoh/1.0.0 (CentOS 7)</address>"
                                        "</body></html>";
    response->setStatus(HttpStatus::HTTP_NOT_FOUND);
    response->setHeader("Server","fepoh/1.0.0");
    response->setHeader("Content-Type","text/html");
    response->setBody(not_found_body);
    return 0;
}

}


}