#include "util.h"
#include "fiber.h"
#include "thread/thread.h"

#include <stddef.h>
#include <thread>
#include <sstream>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

//#include <processthreadsapi.h>

namespace fepoh{
uint32_t GetThreadId(){
    //uint32_t tid = GetCurrentThreadId();
    //return tid;
    return syscall(SYS_gettid);
}

const std::string& GetThreadName(){
    return Thread::GetName();
}

uint32_t GetFiberId(){
    return fepoh::Fiber::GetFiberId();
}

uint64_t GetCurTimeMs(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

uint64_t GetCurTimeUs(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

std::string ErrorToString(){
    std::stringstream ss;
    ss << "errno = " << errno << ":" << strerror(errno);
    return ss.str();
}

} // namespace fepoh

/*
linux获取线程id:<sys/syscall.h> 
    tid syscall(SYS_getid);
windows获取线程id:<processthreadsapi.h> 
    DWORD GetCurrentThreadId()
DWORD就是双字节
*/
