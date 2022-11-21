/*
 * @Author: fepo_h
 * @Date: 2022-11-21 13:07:28
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 13:33:16
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_http_parser.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "http/http_parser.h"
#include "log/log.h"

using namespace fepoh;
static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

/**
 * @description: 测试请求解析,请求有长度才会解析body
 * @return {*}
 */
void test_request(){
    const char data[] = "GET / HTTP/1.1\r\n"
                        "Host:www.baidu.com\r\n"
                        "Accept-Ranges: bytes\r\n"
                        "Cache-Control: no-cache\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length:10\r\n"
                        "Pragma: no-cache\r\n"
                        "Server: BWS/1.1\r\n\r\n"
                        "0123456789";
    http::HttpRequestParser hrp;
    std::string tmp = data;
    hrp.execute(&tmp[0],strlen(data));
    hrp.getData()->dump(std::cout);
}

/**
 * @description: 测试响应解析,响应无长度也会解析body
 * @return {*}
 */
void test_response(){
    const char data[] = "HTTP/1.1 200 OK\r\n"
                        "Accept-Ranges: bytes\r\n"
                        "Cache-Control: no-cache\r\n"
                        "Connection: keep-alive\r\n"
                        "Pragma: no-cache\r\n"
                        "Content-Length:10\r\n"
                        "Server: BWS/1.1\r\n\r\n"
                        "0123456789";
    http::HttpResponseParser hrp;
    std::string tmp = data;
    hrp.execute(&tmp[0],strlen(data));
    hrp.getData()->dump(std::cout);
}

int main(){

    test_request();
    std::cout << "-------" <<std::endl;
    test_response();
}