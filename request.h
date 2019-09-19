#ifndef MY_REQUEST_INCLUDE_H
#define MY_REQUEST_INCLUDE_H

#include <curl/curl.h>
#include <pthread.h>

struct processArg
{
    char *url;
    int count;
    int debug;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

/**
 * 
 * 发送count次请求
 * 
 **/
void *process_request(void* arg);


void multi_thread_process(int totalCount, int ThreadCount, char *url, int debug);

#endif