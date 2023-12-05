//
// Created by Asan on 2023/10/10.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "MinHeap.h"

MinHeap::MinHeap(MinHeapNode *a, int size) : heap_size(size) {
    // 存储首地址
    harr = a;
    // 初始化堆
    int i = (heap_size - 1) / 2;
    while(i >= 0) {
        MinHeapify(i);
        --i;
    }
}

void MinHeap::MinHeapify(int x) {
    int l = left(x), r = right(x);
    int smallest = x;
    if(l < heap_size && harr[l].element < harr[x].element) smallest = l;
    if(r < heap_size && harr[r].element < harr[x].element) smallest = r;
    if(smallest != x){
        swap(&harr[x], &harr[smallest]);
        MinHeapify(smallest);
    }
}

void swap(MinHeapNode *a, MinHeapNode *b){
    MinHeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/* Merge two subarrays of arr[] , arr1[l..m], arr2[m+1..r] */
void merge(int arr[], int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    // 创建临时数组
    int L[n1], R[n2];
    for(i = 0; i < n1; i++)L[i] = arr[l + i];
    for(j = 0; j < n2; j++)R[j] = arr[m + 1 + j];
    i = 0; j = 0; k = l;
    while(i < n1 && j < n2){
        if(L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while(i < n1)arr[k++] = L[i++];
    while(j < n2)arr[k++] = R[j++];
}

/* Merge sort arr[l.....r] */
void mergesort(int arr[], int l, int r){
    if(l < r){
        int m = l + (r - l) / 2;
        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

/* Merge K sorted file, Names of file assumed to be 1, 2, 3, ...k */
void mergeFiles(char *output_file, int n, int k, std::vector<std::string> &files){
    // 打开输入文件数据
    FILE *in[k];
    for(int i = 0; i < k; i++){
        in[i] = openFile( (char *)(files[i].c_str()), "r" );
    }
    // 打开输出文件
    FILE *out = openFile(output_file, "w");
    // 创建小根堆元素
    MinHeapNode *harr = new MinHeapNode[k];
    int i;
    for(i = 0; i < k; i++){
        if(fscanf(in[i], "%d ", &harr[i].element) != 1) break;

        harr[i].i = i;
    }
    // 创建小根堆
    MinHeap hp = MinHeap(harr, k);
    int count = 0;
    /* 循环对k个文件进行归并 */
    while(count != k){
        // 取出当前最小值
        MinHeapNode root = hp.getMin();
        // 将最小值写入输出文件
        fprintf(out, "%d ", root.element);
        // 从最小值的文件中在取出一个数加入堆中
        if(fscanf(in[root.i], "%d ", &root.element) != 1){
            // 说明第i个文件已经取完所有数据
            root.element = INT_MAX;
            ++count;
        }
        hp.replaceMin(root);
    }
    // 关闭文件
    for(int i = 0; i < k; i++)fclose(in[i]);
    fclose(out);
}

/* Merge K sorted file, Names of file assumed to be 1, 2, 3, ...k */
void mergeFiles(char *output_file, int n, int k, int id){
    std::cout << "merging File  : " << id  << " out file : " << output_file << std::endl;
    // 打开输入文件数据
    FILE *in[k];
    for(int i = 0; i < k; i++){
        char filename[5];
        snprintf(filename, sizeof(filename), "%d-%d", id, i);
        in[i] = openFile(filename, "r");
    }
    // 打开输出文件
    FILE *out = openFile(output_file, "w");
    // 创建小根堆元素
    MinHeapNode *harr = new MinHeapNode[k];
    int i;
    for(i = 0; i < k; i++){
        if(fscanf(in[i], "%d ", &harr[i].element) != 1) break;

        harr[i].i = i;
    }
    // 创建小根堆
    MinHeap hp = MinHeap(harr, k);
    int count = 0;
    /* 循环对k个文件进行归并 */
    while(count != k){
        //std::cout << "heap id : " << id << std::endl;
        // 取出当前最小值
        MinHeapNode root = hp.getMin();
        // 将最小值写入输出文件
        fprintf(out, "%d ", root.element);
        // 从最小值的文件中在取出一个数加入堆中
        if(fscanf(in[root.i], "%d ", &root.element) != 1){
            // 说明第i个文件已经取完所有数据
            root.element = INT_MAX;
            ++count;
        }
        hp.replaceMin(root);
    }
    // 关闭文件
    for(int i = 0; i < k; i++)fclose(in[i]);
    fclose(out);
}

/* 对于一个大的文件，将其分成许多个 run_size 的文件进行k路归并 */
int createInitialRuns(char *input_file, int run_size, int id){
    FILE *in = openFile(input_file, "r");
    // 创建输出文件进行k路归并
    char filename[5];
    std::vector<FILE *> out;

    int *arr = (int *)malloc(run_size * sizeof(int));
    bool more_input = true;
    int next_out_file = 0;
    int i;
    while(more_input){
        // 写入run_size个元素到arr数组
        for(i = 0; i < run_size; i++) {
            if( fscanf(in, "%d ", &arr[i]) != 1 ) {
                more_input = false;
                break;
            }
        }
        if(!i)continue;
        // 对子文件进行排序
        mergesort(arr, 0, i - 1);
        snprintf(filename, sizeof(filename), "%d-%d", id, next_out_file);
        out.push_back( openFile(filename, "w") );
        // 写入临时文件
        for(int j = 0; j < i; j++) fprintf(out[next_out_file], "%d ", arr[j]);
        next_out_file++;
    }
    // 关闭文件
    for(int i = 0; i < next_out_file; i++)fclose(out[i]);
    fclose(in);
    return next_out_file;
}

void externalSort(char *input_file, char *output_file, int run_size, int id){
    // 将 "input_file" 分成多个小文件
    int num_ways = createInitialRuns(input_file, run_size, id);
    // 对多个文件进行归并

    mergeFiles(output_file, run_size, num_ways, id);
    std::cout << "merge complet : " << id << std::endl;
}


void test(char * input_file){
    int num_ways = 1, run_size = 10000;
    // 自己造数据
    FILE *in = openFile(input_file, "w");
    srand(time(NULL));
    for(int i = 0; i < num_ways * run_size; i++) fprintf(in, "%d ", rand());
    fclose(in);
    //externalSort(input_file, out_file, num_ways, run_size);
}


FILE *openFile(char *filename, char *mode){
    FILE *fp = fopen(filename, mode);
    if(fp == NULL){
        std::string msg = filename;
        msg += ": Error while opening the file \n";
        perror(msg.c_str());
        exit(EXIT_FAILURE);
    }
    return fp;
}