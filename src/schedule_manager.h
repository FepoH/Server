#include "fiber.h"
#include "thread/mutex.h"
#include "thread/thread.h"
#include "thread/sem.h"

#include <string>
#include <boost/noncopyable.hpp>
#include <atomic>   //原子量,进行多线程操作
#include <functional>
#include <vector>
#include <list>


namespace fepoh{

//同一时刻只能存储回调函数和协程的其中一种
class Task{
    public:
        friend class ScheduleManager;

        Task(std::function<void()> cb);
        Task(Fiber::ptr fiber);
        //如果是回调函数,直接创建Fiber,并返回
        Fiber::ptr getTask();
        //重置,回收高效利用
        void reset();
        //判断是否有任务
        bool isEmpty();

        bool setTask(Fiber::ptr fiber);
        bool setTask(std::function<void()> cb);
    private:
        //空的构造函数,供stl构造,....
        Task(){} 
        std::function<void()> m_cb;
        Fiber::ptr m_fiber;
};

//执行流程的线程只能被一个调度器纳入.
class ScheduleManager : public boost::noncopyable{
    public:
        typedef std::shared_ptr<ScheduleManager> ptr;

        ScheduleManager(const std::string& name="fepoh_scheduler",uint32_t thrCount = 1,bool use_caller = false);
        virtual ~ScheduleManager();
        //启动调度器
        void start();
        //停止调度器,在停止调度器中执行任务
        void stop();
        //批量添加任务
        template<class Iterator>
        void schedule(Iterator begin,Iterator end){
            MutexLock lock(m_mutex);
            while(begin != end){
                scheduleNoLock(*begin);
                ++begin;
            }
        }
        //单个添加任务
        void schedule(Task task);
        void schedule(std::function<void()> cb);
    public:
        //获取线程的协程调度器
        static ScheduleManager* GetThis();
        //设置线程的协程调度器
        static void SetThis(ScheduleManager* val);
        //获取root协程
        static Fiber* GetRootFiber();
    protected:
        //空闲任务
        virtual void idle();
        //是否停止
        virtual bool isStop();
        //通知,唤醒线程
        virtual void notice();
    private:
        //无锁添加任务
        void scheduleNoLock(Task task);
        //线程执行函数
        void run();
        //是否已经有use_caller
        static bool has_use_caller;
    private:
        bool m_isStop = true;               //停止
        //是否执行流程的线程也纳入进去,默认纳入应该好一些,有利于资源的利用
        bool m_useCaller;           
        Mutex m_mutex;                      //锁
        std::string m_name;                 //调度器名称
        uint32_t m_threadCount;             //线程数
        std::list<Task> m_tasks;            //任务池,回调函数或协程,封装后,返回的都是协程
        std::vector<uint64_t> m_threadIds;  //线程id
        std::vector<Thread::ptr> m_threads; //线程池
        std::atomic<uint32_t> m_activeThreadCount = {0};    //活跃的线程数
        std::atomic<uint32_t> m_idleThreadCount = {0};      //空闲的线程数
};


}//namespace
