//
// Created by Asan on 2023/10/9.
//

#ifndef LINUX_SAFEQUEE_H
#define LINUX_SAFEQUEE_H

#include <queue>
#include <mutex>
#include <utility>

/*
 * 使用一个环形队列来存储任务队列.
 * 使用 atomic类 实现CAS无锁线程同步.
 *
*/
#define MAX_SIZE 1024

template<typename T>
class SafeQueue
{
private:
    std::queue<T> queue;
    T data[MAX_SIZE];
    std::atomic<size_t> head{0}, tail{0}, write{0};

public:
    // 初始化CAS
    SafeQueue(){}
    SafeQueue(SafeQueue &&queue){}
    ~SafeQueue(){}

    //判断队列是否为空
    bool empty(){
        //互斥访问队列
        //std::unique_lock<std::mutex> lock(m_mutex);
        return (head.load() == tail.load() );
    }

    //返回队列长度
    int size(){
        //std::unique_lock<std::mutex> lock(m_mutex);
        return (tail.load() - head.load() );
    }

    //将任务入队
    bool enqueue(T &t)
    {
        size_t val, w;
        // CAS算法修改队尾指针
        do{
            val = tail.load();
            if((val + 1) % MAX_SIZE == 0)
                return false;
        }while(!tail.compare_exchange_weak(val, (val + 1) % MAX_SIZE));
        // 新函数加入队尾
        data[val] = t;
        //data[val + 1]();
        // CAS算法跟新队尾入队列
        do{
            w = val;
        }while(!write.compare_exchange_weak(w,(w + 1) % MAX_SIZE));
        //queue.emplace(t);
        return true;
    }
    //队列出队
    bool dequeue(T &t)
    {
        size_t h;
        do{
            h = head.load();
            if(h == write.load())
                return false;
            t = data[h];
        }while( !head.compare_exchange_weak(h, (h + 1)%MAX_SIZE) );
        return true;
    }
};



#endif //LINUX_SAFEQUEE_H
