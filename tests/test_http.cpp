#include "http/http.h"
#include <iostream>

using namespace fepoh::http;

void test_request(){
    HttpRequest hm(0x11,true);
    hm.setPath("/");
    hm.setBody("hello zls");
    hm.setHead("host","www.baidu.com");
    hm.dump(std::cout);
}

void test_response(){
    HttpResponse hr(0x11,true);
    hr.setBody("hello zls");
    hr.setHead("host","www.baidu.com");
    hr.dump(std::cout);
}

int main(){
    test_request();
    test_response();
}