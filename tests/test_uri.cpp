/*
 * @Author: fepo_h
 * @Date: 2022-11-20 21:28:59
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 17:27:31
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_uri.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "uri.h"
#include <iostream>

using namespace fepoh;

void test(){
                                  // https://admin@www.bilibili.com/video/?spm_id_=333.1007.1-1-1&vd_source=100#fragment
    Uri::ptr uri = Uri::CreateUri("https://admin@www.baidu.com/video/?spm_id_=333.1007.1-1-1&vd_source=100#fragment");
    if(uri){
        std::cout << uri->tostring() << std::endl;
    }else{
        std::cout << "parse error" <<std::endl;
    }
    
    Address::ptr addr = uri->createAddr();
    std::cout << addr->tostring() << std::endl;
}



int main(){
    test();
}