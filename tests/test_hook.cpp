/*
 * @Author: fepo_h
 * @Date: 2022-11-20 19:37:54
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 19:46:44
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_hook.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "hook.h"
#include "log/log.h"
#include "io_manager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace fepoh;

static fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("system");


void test_hook(){
    IOManager iom;
    iom.schedule([](){
        sleep(2);
        FEPOH_LOG_DEBUG(s_log_system)<<"sleep2";
    });
    iom.schedule([](){
        sleep(3);
        FEPOH_LOG_DEBUG(s_log_system)<<"sleep3";
    });
    iom.start();
    iom.stop();
}

void test_io(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "112.80.248.75", &addr.sin_addr.s_addr);

    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    FEPOH_LOG_INFO(s_log_system) << "connect rt=" << rt << " errno=" << errno;
    if(rt) {
        return;
    }
    const char data[] = "GET / HTTP/1.1\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    FEPOH_LOG_INFO(s_log_system) << "send rt=" << rt << " errno=" << errno;
    if(rt <= 0) {
        return;
    }
    std::string buff;
    buff.resize(20000);
    FEPOH_LOG_INFO(s_log_system) << "before recv " << errno << "  " <<strerror(errno);
    rt = recv(sock, &buff[0], buff.size(), 0);
    FEPOH_LOG_INFO(s_log_system) << "recv rt=" << rt << " errno=" << errno << "  " <<strerror(errno) ;
    
    if(rt <= 0) {
        return;
    }
    buff.resize(rt);
    FEPOH_LOG_INFO(s_log_system) << buff;
}


int main(int argc ,char * argv[]){
    
    //test_hook();
    if(argc < 2){
        return 0;
    } 
    int port = atoi(argv[1]);
    fepoh::IOManager iom;
    iom.schedule([port](){
        FEPOH_LOG_DEBUG(s_log_system) << "at task";
        test_io(port);
    });
    iom.start();
    return 0;
}