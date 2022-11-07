#pragma once

#include "config.h"
#include <stdint.h>


/*
    全局常数配置,统一管理
*/

namespace fepoh{

//日志相关


//协程相关:
//默认栈大小,128k
static ConfigVar<uint32_t>::ptr g_fiber_stacksize = 
        Config::Lookup<uint32_t>(128*1024,"system.fiber.stacksize","fiber stack size");

static ConfigVar<uint32_t>::ptr g_fiber_root_stacksize = 
        Config::Lookup<uint32_t>(1024*1024,"system.fiber.root.stacksize","fiber root stack size");


//调度器相关
static ConfigVar<uint32_t>::ptr g_iomanager_fd_init = 
        Config::Lookup<uint32_t>(64,"system.iomanager.fdinitsize","system iomanager fd init size");


}//namespace