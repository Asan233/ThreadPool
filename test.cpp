//
// Created by Asan on 2023/10/10.
//

#include <iostream>
#include <random>
#include <vector>
#include "MinHeap.h"
#include "ThreadPool.h"
#include "ReadDirectory.h"

void SoftDir(std::vector<std::string> &filename, ThreadPool &pool)
{
    if(!filename.size())return ;
    std::vector<std::string> outfiles;
    for(int i = 0; i < filename.size(); i++)
    {
        outfiles.push_back(filename[i] + "_out");
    }

    std::vector<decltype(pool.submit(externalSort, (char *)filename[0].c_str(), (char *)outfiles[0].c_str(), 100, 0))> futures;

    for(int i = 0; i < filename.size(); i++)
    {
        futures.push_back( pool.submit(externalSort, (char *)(filename[i].c_str()), (char *)(outfiles[i].c_str()), 100, i) );
    }

    for(auto &ft : futures)
    {
        ft.get();
    }
    // 关闭线程池
    pool.shutdown();
    mergeFiles("out.txt", 100, outfiles.size(), outfiles);
}

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cout << "Input [./test filename]" << std::endl;
        return 0;
    }
    // 创建4个线程的线程池
    ThreadPool pool(4);
    // 初始化线程池
    pool.init();
    std::cout << "ThreadPool Init ..." << std::endl;
    /*
    // 创建测试数据
    for(int i = 0; i < 100; i++)
    {
        std::string name = "./data/" + to_string(i) + ".txt";
        test((char *)name.c_str());
    }
    */
   
    std::vector<std::string> filename;
    int fd = readdir(argv[1], filename);
    for(auto name : filename)
        std::cout << name << std::endl;
    SoftDir(filename, pool);
    
    return 0;
}
