/*
 * @Author: fepo_h
 * @Date: 2022-11-18 11:24:39
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 22:51:46
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http_parser.h
 * @Description: http_parser封装,采用https://github.com/nodejs/http-parser进行解析
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "http.h"
#include "http/http_parser/http_nodejs_parser.h"

namespace fepoh{
namespace http{

/**
 * @description: http请求解析
 * @return {*}
 */
class HttpRequestParser{
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     */    
    HttpRequestParser();
    
    /**
     * @description: 获取HttpRequest
     * @return {*}
     */    
    HttpRequest::ptr getData() {return m_data;}
    /**
     * @description: 获取http_parser
     * @return {*}
     */    
    http_parser getParser() {return m_parser;}
    /**
     * @description: 获取错误
     * @return {*}
     */    
    const int getError() const {return m_error;}
    /**
     * @description: 设置error
     * @return {*}
     * @param {int} v
     */    
    void setError(int v) {m_error = v;}
    /**
     * @description: http_parser将首部的key-value分两部分解析,field保存key
     * @return {*}
     */    
    const std::string getField() const {return m_field;}
    /**
     * @description: http_parser将首部的key-value分两部分解析,field保存key
     * @return {*}
     * @param {string&} v
     */    
    void setField(const std::string& v) {m_field = v;}

    /**
     * @description: 执行转换
     * @return {*}
     * @param {char} *data http数据
     * @param {size_t} len 长度
     */    
    int execute(char *data, size_t len);
    /**
     * @description: 转换是否完成
     * @return {*}
     */    
    int isFinished() const {return m_isFinished;}
    /**
     * @description: 设置转换是否完成
     * @return {*}
     * @param {bool} v
     */    
    void setFinished(bool v) {m_isFinished = v;}
    /**
     * @description: 获取body
     * @return {*}
     */    
    const std::string& getBody() {return m_body;}
    /**
     * @description: 增加body内容
     * @return {*}
     * @param {char*} buffer
     * @param {int} length
     */    
    void appendBody(const char* buffer,int length);

    //TODO
    bool getHeadFinish() const {return m_headFinish;}
    void setHeadFinish(bool v) {m_headFinish = v;}
    /**
     * @description: 获取首部最大长度
     * @return {*}
     */    
    static uint64_t GetRequestHeadBufSize();
    /**
     * @description: 获取body最大长度
     * @return {*}
     */    
    static uint64_t GetRequestBodyBufSize();
    /**
     * @description: 获取body长度
     * @return {*}
     */    
    uint64_t getContentLength();
private:
    HttpRequest::ptr m_data;        //请求
    http_parser m_parser;           //parser
    int m_error;                    //错误
    std::string m_field;            //首部key
    bool m_isFinished;              //是否完成
    std::string m_body;             //body
    bool m_headFinish = false;      //头部是否解析完成
};

/**
 * @description: http响应解析
 * @return {*}
 */
class HttpResponseParser{
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     */    
    HttpResponseParser();
    /**
     * @description: 获取HttpResponse
     * @return {*}
     */    
    HttpResponse::ptr getData() {return m_data;}
    /**
     * @description: 获取http_parser
     * @return {*}
     */    
    const http_parser getParser() const {return m_parser;}
    /**
     * @description: 获取错误
     * @return {*}
     */    
    const int getError() const {return m_error;}
    /**
     * @description: 设置错误
     * @return {*}
     * @param {int} v
     */    
    void setError(int v) {m_error = v;}
    /**
     * @description: 同HttpRequestParser
     * @return {*}
     */    
    const std::string getField() const {return m_field;}
    /**
     * @description: 同HttpRequestParser
     * @return {*}
     * @param {string&} v
     */    
    void setField(const std::string& v) {m_field = v;}
    /**
     * @description: 获取body长度
     * @return {*}
     */    
    uint64_t getContentLength();
    /**
     * @description: 获取body
     * @return {*}
     */    
    const std::string& getBody() {return m_body;}
    /**
     * @description: 追加body数据
     * @return {*}
     * @param {char*} buffer
     * @param {int} length
     */    
    void appendBody(const char* buffer,int length);
    /**
     * @description: 执行转换
     * @return {*}
     * @param {char} *data http数据
     * @param {size_t} len
     */    
    int execute(char *data, size_t len);
    /**
     * @description: 是否有错误
     * @return {*}
     */    
    int hasError() const {return m_error;}
    /**
     * @description: 是否完成转换
     * @return {*}
     */    
    bool isFinished() const {return m_isFinished;}
    /**
     * @description: 设置是否完成转换
     * @return {*}
     * @param {bool} v
     */    
    void setFinished(bool v) {m_isFinished = v;}
    /**
     * @description: 是否完成首部解析
     * @return {*}
     */    
    bool getHeadFinish() const {return m_headFinish;}
    /**
     * @description: 设置是否完成首部解析
     * @return {*}
     * @param {bool} v
     */    
    void setHeadFinish(bool v) {m_headFinish = v;}

    /**
     * @description: 获取http响应首部最大长度
     * @return {*}
     */    
    static uint64_t GetResponseHeadBufSize();
    /**
     * @description: 获取http响应body最大长度
     * @return {*}
     */    
    static uint64_t GetResponseBodyBufSize();
private:
    HttpResponse::ptr m_data;       //HttpResponse
    http_parser m_parser;           //http_parser
    int m_error;                    //错误
    bool m_isFinished;              //是否完成
    std::string m_field;            //首部field
    std::string m_body;             //body
    bool m_headFinish = false;      //首部是否解析完成
};


}

}


