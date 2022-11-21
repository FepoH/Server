/*
 * @Author: fepo_h
 * @Date: 2022-11-20 22:22:13
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 22:26:51
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_http.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "http/http.h"
#include <iostream>

using namespace fepoh::http;

void test_request(){
    HttpRequest hm(0x11,true);
    hm.setPath("/");
    hm.setBody("hello zls");
    hm.setHeader("host","www.baidu.com");
    std::cout << hm.tostring() <<std::endl;
}

void test_response(){
    HttpResponse hr(0x11,true);
    hr.setBody("hello zls");
    hr.setStatus(HttpStatus::HTTP_OK);
    hr.setHeader("host","www.baidu.com");
    std::cout << hr.tostring() <<std::endl;
}

int main(){
    test_request();
    test_response();
}