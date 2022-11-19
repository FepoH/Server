/*
 * @Author: fepo_h
 * @Date: 2022-11-19 21:31:57
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 21:53:43
 * @FilePath: /fepoh/workspace/fepoh_server/src/util.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>


using Json = nlohmann::json;

namespace fepoh{

uint32_t GetThreadId();

const std::string& GetThreadName();

uint32_t GetFiberId();

uint64_t GetCurTimeMs();

uint64_t GetCurTimeUs();

//判断是否是一个基础类型:此处基础类型为数字,布尔值和空,字符串不纳入
template<class T>
bool IsBaseType(const T& val){
    try{
        Json js(val);
        return js.is_boolean()||js.is_number()||js.is_string();
    }catch(...){
        return false;
    }
    return false;
}

std::string ErrorToString();

std::string BackTrace(int size = 64,int skip = 1,const std::string& prefix = "");

void BackTrace(std::vector<std::string>& vecStr, int size = 64,int skip = 1);

}//namespace