#include "request.h"
#include<stdlib.h>
#include<string.h>
extern pthread_mutex_t mutex;
extern int global_Success;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    char* content = NULL;
    //debug输出响应内容
    if (*(int*)userdata == 1){
        content = malloc(size * nmemb + 1);
        memcpy(content, ptr, size * nmemb);
        content[size * nmemb] = 0;
        printf(content);
        free(content);
    }
  
    return size * nmemb; 
}


void *process_request(void* v)
{
    CURL *curl;
    CURLcode res;

    char *url;
    int count;
    int debug;

    int success;
    
    //检查参数
    struct processArg *arg;
    arg = (struct processArg *)v;
    url = arg->url;
    count =  arg->count;
    debug = arg->debug;

    free(v);

    success = 0;

    //init
    curl = curl_easy_init();
    if(curl) {
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &debug);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2000L);
    
        int i;
        for(i = 0; i < count; i++)
        {
            /* Perform the request, res will get the return code */ 
            res = curl_easy_perform(curl);
            /* Check for errors */ 
            if(res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            }
            else
            {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                // printf("response code: %lu \n", response_code);
                if (response_code == 200)
                {
                    success++;
                }
            }
        }
        /* 每次会话结束时调用,和 curl_easy_init成对出现*/ 
        curl_easy_cleanup(curl);

        //全局统计
        pthread_mutex_lock(&mutex);
        global_Success = global_Success + success;
        pthread_mutex_unlock(&mutex);
    }
}




void multi_thread_process(int totalCount, int threadCount, char *url, int debug){
    pthread_t threads[threadCount];
    int remain_cnt,avg_cnt;

    remain_cnt = totalCount % threadCount;
    avg_cnt = totalCount / threadCount;

    struct processArg *arg;
    arg = NULL;

    int i;
    for(i = 0; i < threadCount; i++)
    {
        arg = malloc(sizeof(struct processArg));
        arg->url = url;
        arg->count = avg_cnt;
        arg->debug = debug;
        if (i == threadCount - 1){
            arg->count = avg_cnt + remain_cnt;
        }
        if (pthread_create(&threads[i], NULL, (void *)process_request, (void *)arg) != 0){
            printf("线程创建失败\n");
        }
    }

    void* ret;
    for(i = 0; i < threadCount; i++)
    {
        if (pthread_join(threads[i],&ret) != 0)
        {
            printf("cannot join with thread[%d]\n",i);
        }
    }
}