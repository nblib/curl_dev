#include <stdio.h>
#include <curl/curl.h>
#include "request.h"



pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int global_Success;



int THREAD_COUNT = 20; // 启用线程数
int TOTAL_COUNT = 100;  // 总请求数量
char *URL = "https://example.com"; // url
int DEBUG = 0; //1输出响应内容,0不输出

int main(int argc, char *argv[])
{
  //statistic init
  pthread_mutex_init(&mutex, NULL);
  //libcurl init
  curl_global_init(CURL_GLOBAL_DEFAULT);
  

  //request
  multi_thread_process(TOTAL_COUNT,THREAD_COUNT,URL,DEBUG);
 

  //cleanup
  curl_global_cleanup();
  //destroy mutex
  pthread_mutex_destroy(&mutex);

  //output statistic
  printf("total: %d, success: %d\n",TOTAL_COUNT, global_Success);

  return 0;
}
