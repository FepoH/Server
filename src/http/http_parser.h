#pragma once

#include "http/http_parser/http_nodejs_parser.h"
#include "http.h"

namespace fepoh{
namespace http{

class HttpRequestParser{
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;

    HttpRequestParser();

    HttpRequest::ptr getData() {return m_data;}
    http_parser getParser() {return m_parser;}
    const int getError() const {return m_error;}
    void setError(int v) {m_error = v;}
    const std::string getField() const {return m_field;}
    void setField(const std::string& v) {m_field = v;}

    int execute(char *data, size_t len);
    int isFinished() const {return m_isFinished;}
    void setFinished(bool v) {m_isFinished = v;}
    const std::string& getBody() {return m_body;}
    void appendBody(const char* buffer,int length);

    bool getHeadFinish() const {return m_headFinish;}
    void setHeadFinish(bool v) {m_headFinish = v;}

    static uint64_t GetRequestHeadBufSize();
    static uint64_t GetRequestBodyBufSize();

    uint64_t getContentLength();
private:
    HttpRequest::ptr m_data; //请求
    http_parser m_parser;   //parser
    int m_error;            //错误
    std::string m_field;    //头部field
    bool m_isFinished;
    std::string m_body;
    bool m_headFinish = false;      
};


class HttpResponseParser{
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;

    HttpResponseParser();

    HttpResponse::ptr getData() {return m_data;}
    const http_parser getParser() const {return m_parser;}
    const int getError() const {return m_error;}
    void setError(int v) {m_error = v;}

    const std::string getField() const {return m_field;}
    void setField(const std::string& v) {m_field = v;}

    uint64_t getContentLength();
    const std::string& getBody() {return m_body;}
    void appendBody(const char* buffer,int length);

    int execute(char *data, size_t len);
    int hasError() const {return m_error;}
    bool isFinished() const {return m_isFinished;}
    void setFinished(bool v) {m_isFinished = v;}
    bool getHeadFinish() const {return m_headFinish;}
    void setHeadFinish(bool v) {m_headFinish = v;}

    static uint64_t GetResponseHeadBufSize();
    static uint64_t GetResponseBodyBufSize();
private:
    HttpResponse::ptr m_data;
    http_parser m_parser;
    int m_error;
    bool m_isFinished;
    std::string m_field;    //头部field
    std::string m_body;
    bool m_headFinish = false;
};


}

}


