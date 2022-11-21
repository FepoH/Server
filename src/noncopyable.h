/*
 * @Author: fepo_h
 * @Date: 2022-11-10 09:04:16
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 21:34:33
 * @FilePath: /fepoh/workspace/fepoh_server/src/noncopyable.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once


class Noncopyable{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete; 
};