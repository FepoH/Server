#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include "http.h"
#include "http_session.h"
#include "thread/mutex.h"

namespace fepoh{
namespace http{

class Servlet{
public:
    typedef std::shared_ptr<Servlet> ptr;

    Servlet(const std::string& name):m_name(name){}
    virtual ~Servlet(){}
    virtual int32_t handle(HttpRequest::ptr request
                            ,HttpResponse::ptr response
                            ,HttpSession::ptr session) = 0;
    
    const std::string& getName() const {return m_name;}
protected:
    std::string m_name;
};

class FunctionServlet : public Servlet{
public:
    typedef  std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t(HttpRequest::ptr request
                            ,HttpResponse::ptr response
                            ,HttpSession::ptr session)> callbck;
    
    FunctionServlet(callbck cb);

    int32_t handle(HttpRequest::ptr request
            ,HttpResponse::ptr rsponse,HttpSession::ptr session) override;
private:
    callbck m_cb;
};

class NotFoundServlet : public Servlet{
public:
    typedef  std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet():Servlet("NotFoundServlet"){}

    int32_t handle(HttpRequest::ptr request
            ,HttpResponse::ptr rsponse,HttpSession::ptr session) override;
};

class ServletDispath : public Servlet{
public:
    typedef std::shared_ptr<ServletDispath> ptr;

    typedef std::function<int32_t(HttpRequest::ptr request
                        ,HttpResponse::ptr response
                        ,HttpSession::ptr session)> callbck;
    ServletDispath();

    int32_t handle(HttpRequest::ptr request,HttpResponse::ptr response
                            ,HttpSession::ptr session);

    void addServlet(const std::string& uri,Servlet::ptr slt);
    void addServlet(const std::string& uri,FunctionServlet::callbck cb);
    void addGlobServlet(const std::string& uri,Servlet::ptr slt);
    void addGlobServlet(const std::string& uri,FunctionServlet::callbck cb);

    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);


    void setDefault(Servlet::ptr v) {m_default = v;}
    Servlet::ptr getDefault() {return m_default;}

    Servlet::ptr getServlet(const std::string& uri);
    Servlet::ptr getGlobServlet(const std::string& uri);

    Servlet::ptr getMatchServlet(const std::string& uri);
private:
    RWMutex m_mutex;
    std::unordered_map<std::string,Servlet::ptr> m_data;

    std::vector<std::pair<std::string,Servlet::ptr>> m_globData;
    Servlet::ptr m_default;
};

}


}