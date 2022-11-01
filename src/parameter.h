#pragma once

#include <stdint.h>

namespace fepoh{

//日志相关




//协程相关:
//默认栈大小,128k
const uint32_t g_fiber_stacksize = 128 * 1024;
const uint32_t g_fiber_root_stacksize = 1024 *1024;

//调度器相关
const uint32_t g_iomanager_fd_init = 64;

}//namespace