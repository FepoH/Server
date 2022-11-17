#pragma once

#include <cassert>
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
        assert(false);

#define FEPOH_ASSERT1(condi,descri)  \
    if(FEPOH_UNLIKELY(!(condi))){    \
        FEPOH_LOG_ERROR(FEPOH_LOG_NAME("system")) << std::string("") + #descri + "assert::" #condi; \
        assert(false);    \
    }

