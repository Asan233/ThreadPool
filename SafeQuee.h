//
// Created by Asan on 2023/10/9.
//

#ifndef LINUX_SAFEQUEE_H
#define LINUX_SAFEQUEE_H

#include <queue>
#include <mutex>
#include <utility>

/*
 * 使用STL标准库中Queue来维护一个 Task Queue，来对Task Queue进行线程安全的访问
 *
*/
template<typename T>
class SafeQueue
{
private:
    std::queue<T> m_queue;      //任务队列
    std::mutex m_mutex;   //队列访问互斥量

public:
    SafeQueue(){}
    SafeQueue(SafeQueue &&queue){}
    ~SafeQueue(){}

    //判断队列是否为空
    bool empty(){
        //互斥访问队列
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    //返回队列长度
    int size(){
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    //将任务入队
    void enqueue(T &t)
    {
        // 加锁
        std::unique_lock<std::mutex> lock(m_mutex);
        // 入队
        m_queue.emplace(t);
    }
    //队列出队
    bool dequeue(T &t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        // 取出队首元素，返回队首元素并右值引用
        t = std::move(m_queue.front());
        // 队首元素出队
        m_queue.pop();
        return true;
    }
};



#endif //LINUX_SAFEQUEE_H
