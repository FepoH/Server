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

}//namespace