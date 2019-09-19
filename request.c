#include "request.h"
#include<stdlib.h>
#include<string.h>
#include<uuid/uuid.h>
extern pthread_mutex_t mutex;
extern int global_Success;

int str_contain_char(char *src, char target);
char* url_add_uuid(char *rawUrl);

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

/**
 * 发起网络请求
 * */
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

        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2000L);
    
        int i;
        for(i = 0; i < count; i++)
        {
            //url add uuid
            char *uuid_url = url_add_uuid(url);
            if (uuid_url == NULL)
            {
                //申请失败
                continue;
            }
            
    
            curl_easy_setopt(curl, CURLOPT_URL, uuid_url);
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
            //free
            free(uuid_url);
        }
        /* 每次会话结束时调用,和 curl_easy_init成对出现*/ 
        curl_easy_cleanup(curl);

        //全局统计
        pthread_mutex_lock(&mutex);
        global_Success = global_Success + success;
        pthread_mutex_unlock(&mutex);
    }
}



/**
 * 多线程运行,并等待返回
 * */
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

/**
 * 判断src中是否包含target
 * */
int str_contain_char(char *src, char target)
{
    int rst = 0;
    
    if (src == NULL || *src == '\0'){
        return rst;
    }

    while (*src != '\0')
    {
        if (*src == target)
        {
            rst = 1;
            break;
        }
        src++;
    }
    return rst;
}
/**
 * 给url添加uuid参数,返回字符串需要手动释放, 申请空间失败返回NULL
 * */
char* url_add_uuid(char *rawUrl)
{
    uuid_t uuid;
    char uuidstr[36];
 
    uuid_generate(uuid);
    uuid_unparse(uuid, uuidstr); 

    int raw_url_len = strlen(rawUrl);
    char* uuid_str = malloc(raw_url_len + 1 + 1 + 4 + 1 + 36);
    if (uuid_str == NULL)
    {
        return NULL;
    }
    memcpy(uuid_str, rawUrl, raw_url_len + 1);
    //不以/结尾
    if (uuid_str[raw_url_len - 1] != '/')
    {
        //是否?结尾
        if (uuid_str[raw_url_len - 1] == '?')
        {
            memcpy(uuid_str + raw_url_len, "uuid=", 5);
            raw_url_len += 5;
            memcpy(uuid_str + raw_url_len,uuidstr, 36);
             raw_url_len += 36;
            uuid_str[raw_url_len] = 0;
        }else{
            if(str_contain_char(uuid_str,'?') == 1)
            {
                memcpy(uuid_str + raw_url_len, "&uuid=",6);
                raw_url_len += 6;
                memcpy(uuid_str + raw_url_len,uuidstr, 36);
                raw_url_len += 36;
                uuid_str[raw_url_len] = 0;
            }else
            {
                memcpy(uuid_str + raw_url_len, "?uuid=",6);
                raw_url_len += 6;
                memcpy(uuid_str + raw_url_len,uuidstr, 36);
                raw_url_len += 36;
                uuid_str[raw_url_len] = 0;
            }
        }
    }
    return uuid_str;
}