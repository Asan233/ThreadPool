#ifndef __READ_H
#define __READ_H

#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int readdir(string path, vector<string> &fn)
{
    // 打开目录文件
    DIR* dirp = opendir(path.c_str());
    if(!dirp) 
    {
        std::cout << "open file" << path << "error" << std::endl;
        return -1;
    }
    struct stat st;
    struct dirent *dir;
    //读取目录文件
    while( (dir = readdir(dirp) ) != NULL)
    {
        // 跳过隐藏文件
        if(dir->d_name[0] == '.')continue;
        string full_path = path + dir->d_name;
        if(lstat(full_path.c_str(), &st) == -1)continue;
        fn.push_back(full_path);
    }
    return 0;
}

#endif
