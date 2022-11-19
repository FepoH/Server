/*
 * @Author: fepo_h
 * @Date: 2022-11-19 21:50:12
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 21:52:34
 * @FilePath: /fepoh/workspace/fepoh_server/src/macro.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <cassert>
#include "util.h"
#include "log/log.h"

//
#if  defined __GNUC__ || defined __llvm
#define FEPOH_LIKELY(x)             __builtin_expect(!!(x),1)
#define FEPOH_UNLIKELY(x)           __builtin_expect(!!(x),0)
#else
#define FEPOH_LIKELY(x)         (x)
#define FEPOH_LIKELY(x)         (x)
#endif

//断言
#define FEPOH_ASSERT(condi)  \
    if(FEPOH_UNLIKELY(!(condi)))    \
        FEPOH_LOG_ERROR(FEPOH_LOG_NAME("system")) << "BackTrace:" << fepoh::BackTrace();   \
        assert(false);

#define FEPOH_ASSERT1(condi,descri)  \
    if(FEPOH_UNLIKELY(!(condi))){    \
        FEPOH_LOG_ERROR(FEPOH_LOG_NAME("system")) << std::string("") + #descri + "assert::" #condi \
                    << std::endl << "BackTrace:" << fepoh::BackTrace();    \
        assert(false);    \
    }

