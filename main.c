#include <stdio.h>
#include <curl/curl.h>
#include "request.h"







int THREAD_COUNT = 1; // 启用线程数
int TOTAL_COUNT = 1;  // 总请求数量
char *URL = "https://example.com"; // url
int DEBUG = 1; //1输出响应内容,0不输出

int main(int argc, char *argv[])
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
  
  //request
  multi_thread_process(TOTAL_COUNT,THREAD_COUNT,URL,1);
 
  curl_global_cleanup();
 
  return 0;
}