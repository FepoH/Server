/*
 * @Author: fepo_h
 * @Date: 2022-11-10 00:10:28
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:05:05
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_address.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "address.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_address(){
    
    Address::ptr addr = Address::LookupIPAddr("www.baidu.com",0);
    if(!addr){
        FEPOH_LOG_DEBUG(s_log_system) << "Fail";
        return ;
    }
    FEPOH_LOG_DEBUG(s_log_system) << addr->tostring();

    std::vector<IPAddress::ptr> vecIPAddr;
    bool flag = IPAddress::Lookup(vecIPAddr,"www.baidu.com:http",0);
    if(flag){
        for(auto item:vecIPAddr){
            FEPOH_LOG_DEBUG(s_log_system) << item->tostring();
        }
    }
    
}

void test_itfc(){
    std::multimap<std::string,std::pair<Address::ptr,uint32_t>> res;
    Address::GetInterfaceAddresses(res);
    for(auto it = res.begin();it != res.end();++it){
        std::cout << it->first << "   " <<it->second.first->tostring() << "   " << it->second.second<<std::endl;
    }
}

void test1(){
    struct addrinfo hints;
    struct addrinfo* res,*rp;
    bzero(&hints ,sizeof(hints));
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = 0;
    hints.ai_socktype = 0;


    int rt = getaddrinfo("www.baidu.com",NULL ,&hints ,&res);
    if(rt == 0){
        FEPOH_LOG_DEBUG(s_log_system) << "success" << rt ;
    }else{
        FEPOH_LOG_DEBUG(s_log_system) << "fail" <<rt <<" "<<errno <<strerror(errno);
    }
}

int main(){
    test_address();
    test1();
    test_itfc();
    return 0;
}