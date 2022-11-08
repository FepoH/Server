#pragma once

#include <cassert>
#include "log/log.h"

#define FEPOH_ASSERT(condi)  \
    if(!(condi))    \
        assert(false);

#define FEPOH_ASSERT1(condi,descri)  \
    if(!(condi)){    \
        FEPOH_LOG_ERROR(FEPOH_LOG_NAME("system")) << std::string("") + #descri + "assert::" #condi; \
        assert(false);    \
    }

