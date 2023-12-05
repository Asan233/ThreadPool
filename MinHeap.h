//
// Created by Asan on 2023/10/10.
//

#ifndef LINUX_MINHEAP_H
#define LINUX_MINHEAP_H

#include <cstdio>
#include <time.h>
#include <cstdlib>
#include <climits>
#include <string>

struct MinHeapNode{
    int element;    // 被排序的元素
    int i;     // 元素在其数组中索引
};

void swap(MinHeapNode *x, MinHeapNode *y);      // 小根堆节点交换函数

class MinHeap{
    MinHeapNode *harr;  // 小根堆数组指针
    int heap_size;      // 堆大小
public:
    MinHeap(MinHeapNode a[], int size);      // 初始化一个size大小的堆
    void MinHeapify(int);                    // 对给定数组索引进行堆化
    int left(int i){ return (2 * i + 1); }    // 返回i的左孩子索引
    int right(int i){ return (2 * (i + 1)); }   // 返回i的右孩子索引
    MinHeapNode getMin() {return harr[0];}      // 返回堆顶节点
    // 用x节点替代堆顶节点
    void replaceMin(MinHeapNode x) {
        harr[0] = x;
        MinHeapify(0);
    }
};

void merge(int arr[], int l, int m, int r);        // merge arr l[] and r[]
void mergesort(int arr[], int l, int r);     // merge sort
void mergeFiles(char *output_File, int n, int k, int id);   // merge sort file
void mergeFiles(char *output_file, int n, int k, std::vector<std::string> &files); // merge sort file
int createInitialRuns(char *input_file, int run_size, int num_ways, int id);    // using a merge algorithm to sort file
void externalSort(char *input_file, char *output_file, int run_size, int id);     // externalSort
void test(char * input);    // test externalsort


// open file
FILE *openFile(char *filename, char *mode);

#endif //LINUX_MINHEAP_H
