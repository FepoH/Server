/*
 * @Author: fepo_h
 * @Date: 2022-11-19 23:33:33
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 23:35:25
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_json.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "log/log.h"
#include "macro.h"

#include <nlohmann/json.hpp>
#include <istream>

using namespace fepoh;

using Json = nlohmann::json;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_json(){
    std::ifstream in("/home/fepoh/workspace/fepoh_server/tests/test_json.json",std::ios::binary);
    if(!in.is_open()){
        FEPOH_LOG_ERROR(s_log_system) << "open file error.";
        return ;
    }
    Json j = Json::parse(in);
    if(j.contains("port")){
        std::cout << j["port"] <<std::endl;
    }else{
        std::cout << "parser error" <<std::endl;
    }
}
/**
 * @description: 自定义类型转换测试
 * @return {*}
 */
class Person{
public:
    std::string name;
    std::string address;
    int age;
};

 void to_json(Json& j, const Person& p) {
    j = Json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
}

 void from_json(const Json& j, Person& p) {
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
}

void test_selfdefine(){
    Person p = {"zhou le sha","jiahe",23};
    Json j(p);
    std::cout << j <<std::endl;
    auto p2 = j.get<Person>();
}


int main(){
    test_json();
    test_selfdefine();
}