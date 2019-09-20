#include <stdio.h>
#include <curl/curl.h>
#include "request.h"
#include<stdlib.h>
#include<time.h>
#include<string.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int global_Success;



long THREAD_COUNT = 1; // 启用线程数
long TOTAL_COUNT = 1;  // 总请求数量
char *URL = "https://example.com"; // url
int DEBUG = 0; //1输出响应内容,0不输出


char *useage = "curl_dev [debug] [totalcount] [threadcount] url\n比如: curl_dev https://www.example.com  将会默认请求一次使用一个线程\n curl_dev 20 https://www.example.com  将会请求20次使用一个线程\n curl_dev 20 10 https://www.example.com 总共请求20次使用10个线程\n curl_dev debug 20 10 https://www.example.com 开启调试模式,输出响应内容\n";

int arg_parse(int argc, char* argv[])
{
  if (argc <= 1)
  {
    printf("require url\n");
    printf(useage);
    return -1;    
  }
  int i;
  for(i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "debug") == 0)
    {
      DEBUG = 1;
    }
    else if(('0' <= argv[i][0]) && ('9' >= argv[i][0]))
    {
      long count = strtol(argv[i], NULL, 10);
      if (count > 0)
      {
          if(TOTAL_COUNT <= 1)
          {
            TOTAL_COUNT = count;
          }
          else
          {
              THREAD_COUNT = count;
          }
      }
    }
    else
    {
      URL = argv[i];
    }
  }
  return 0;
}
int main(int argc, char *argv[])
{
  //参数处理
  if(arg_parse(argc, argv) != 0)
  {
    return 1;
  }

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
