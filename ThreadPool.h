//
// Created by Asan on 2023/10/9.
//
#ifndef LINUX_THREADPOOL_H
#define LINUX_THREADPOOL_H
#include <thread>
#include <future>
#include <vector>
#include <functional>
#include <utility>
#include <atomic>
#include "SafeQuee.h"

class ThreadPool
{
private:
    /**
     *      内置工资线程类执行真正的工作
     * */
    class ThreadWorker
    {
    private:
        // work id
        int m_id;

        // 线程池
        ThreadPool *m_pool;
    public:
        ThreadWorker(ThreadPool *pool, const int id) : m_pool(pool), m_id(id){}

        // 重载()操作以执行真正任务
        void operator() ()
        {
            bool dequeued;      // 是否取出任务标志
            std::function<void()> func;     // 基础任务函数

            // 线程池开启时，循环取出队列任务工作
            while(!m_pool->m_shutdown)
            {
                {
                    // 线程环境加锁，互斥访问工作线程的休眠与唤醒
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);

                    // 队列为空则休眠线程
                    if( m_pool->m_queue.empty() )
                    {
                        m_pool->m_conditional_lock.wait(lock);
                    }

                    //取出队列
                    dequeued = m_pool->m_queue.dequeue(func);
                }
                // 成功取出则执行
                if( dequeued ){
                    printf("dequeue work ! \n");
                    func();
                }
            }
        }

    };

    bool m_shutdown;    // 线程池是否关闭
    SafeQueue<std::function<void()>> m_queue;   //任务队列
    std::mutex m_conditional_mutex; // 线程休眠锁互斥变量
    std::condition_variable m_conditional_lock; //让线程处于休眠或唤醒
    std::vector<std::thread> m_threads;         //工作线程队列

public:
    /*
     *  sumbit()函数用于接收任何函数，并负责向队列添加任务
     *  F : 任务函数
     *  Args : 任务函数参数
     * */
    // 尾部函数返回值从 future<decltype(f(args...))>自动推导
    // future 获取异步线程结果
    template<typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        // 将f(arg...)bind然后用function返回一个函数f(args...),不含参数的通用函数func
        // forward完美转发避免左右值错误
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        // 封装任务指针实现异步调用
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // 进一步将func()封装成一个void ()函数
        std::function<void()> warpper_func = [task_ptr](){ (*task_ptr)(); };

        // 任务函数入队
        m_queue.enqueue(warpper_func);
        printf("work enqueue !\n");

        // 唤醒等待中的线程
        m_conditional_lock.notify_one();

        // 返回任务指针
        return task_ptr->get_future();
    }

    // 线程池构造函数，传入构造线程数量，默认4个线程
    ThreadPool(const int n_threads = 4) : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false){}

    /*只能通过构造函数初始化*/
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;
    void init();    //初始化线程池
    void shutdown();    // 关闭线程池
};
#endif //LINUX_THREADPOOL_H
