//
// Created by Asan on 2023/10/9.
//

#include "ThreadPool.h"

void ThreadPool::init(){
        for(int i = 0; i < m_threads.size(); ++i)
        {
            // 创建工作线程
            m_threads.at(i) = std::thread(ThreadWorker(this, i));
        }
}

void ThreadPool::shutdown() {
    m_shutdown = true;
    m_conditional_lock.notify_all();    // 唤醒所有线程

    for(int i = 0; i < m_threads.size(); ++i)
    {
        // 线程是否等待，没有则加入等待队列
        if(m_threads.at(i).joinable())
        {
            m_threads.at(i).join();
        }
    }
}




