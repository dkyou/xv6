// sleep.c 
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int time;
  if(argc != 2){           // 输入的参数数量错误，报错并退出 
    fprintf(2, "=======sleep: usage: sleep <time>\n");
    exit(1);
  }                 
  time = atoi(argv[1]);	   // 将输入的时间转化为整数，调用sleep完成功能
  printf("delay %d s\n",time);
  sleep(time);
  exit(0);
}
