/*
 * @Author: fepo_h
 * @Date: 2022-11-19 21:31:57
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 21:40:01
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
/**
 * @description: 获取当前线程id
 * @return {*}
 */
uint32_t GetThreadId();

/**
 * @description: 获取当前线程名
 * @return {*}
 */
const std::string& GetThreadName();

/**
 * @description: 获取当前协程id
 * @return {*}
 */
uint32_t GetFiberId();

/**
 * @description: 获取当前事件(毫秒级精度)
 * @return {*}
 */
uint64_t GetCurTimeMs();

/**
 * @description: 获取当前时间(微秒级精度)
 * @return {*}
 */
uint64_t GetCurTimeUs();

/**
 * @description: 判断是否是一个基础类型:此处基础类型为数字,布尔值和空,字符串不纳入,用于Config
 * @return {*}
 */
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

/**
 * @description: 将errno装字符串
 * @return {*}
 */
std::string ErrorToString();

/**
 * @description: 回溯
 * @return {*}
 * @param {int} size 最多回溯size层
 * @param {int} skip 跳过的层数
 * @param {string&} prefix 前缀
 */
std::string BackTrace(int size = 64,int skip = 1,const std::string& prefix = "");

/**
 * @description: 回溯重载
 * @return {*}
 * @param {std::vector<std::string>&} 结果集
 * @param {int} size 
 * @param {int} skip
 */
void BackTrace(std::vector<std::string>& vecStr, int size = 64,int skip = 1);

}//namespace