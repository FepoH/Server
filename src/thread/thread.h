#pragma once

#include "noncopyable.h"
#include <pthread.h>
#include <functional>
#include <memory>

namespace fepoh{

    class Thread : Noncopyable{
        public:
            typedef std::shared_ptr<Thread> ptr;

            Thread(std::function<void()> cb,const std::string& m_name);
            ~Thread();
            //join线程
            void join();
            //将线程detach
            void detach();
            //获取线程id
            uint64_t getId() const {return m_id;}
            //获取线程名
            const std::string& getName() const {return m_name;}
            //获取当前线程
            static Thread* GetThis();
            static const std::string& GetName();
            static void SetName(const std::string& name);
        private:
            //公用的线程调用函数,在此函数内调用cb
            static void* Run(void* arg);
        private:
            uint64_t m_id = 0;          //线程id
            std::string m_name;         //线程名
            pthread_t m_thread;         //线程
            std::function<void()> m_cb; //回调函数

    };

}