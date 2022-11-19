#include "uri.h"
#include <iostream>

using namespace fepoh;

void test(){
                                  // https://admin@www.bilibili.com/video/?spm_id_=333.1007.1-1-1&vd_source=100#fragment
    Uri::ptr uri = Uri::CreateUri("https://admin@www.baidu.com/video/?spm_id_=333.1007.1-1-1&vd_source=100#fragment");
    uri->dump(std::cout);
    Address::ptr addr = uri->createAddr();
    std::cout << addr->tostring() << std::endl;
}



int main(){
    test();
}