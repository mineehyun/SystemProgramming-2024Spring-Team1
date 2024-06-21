#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include "rp2.h"
#include "pi2_weatherThreads.c"

#define HG 27
#define HR 22
#define C_G 24
#define C_R 23
#define time_D 5

int flag, weak_button = 0; 

void* human(int time){
  flag = 1;
  GPIOWrite(HR, 0);
  GPIOWrite(HG, 1);
  sleep(time);
  GPIOWrite(HR, 1);
  sleep(3);
  GPIOWrite(HG, 0);
  return NULL;
}

void* car(int time){
  flag = 0;
  GPIOWrite(C_R, 0);
  GPIOWrite(C_G, 1);
  sleep(time);
  GPIOWrite(C_R, 1);
  sleep(3);
  GPIOWrite(C_G, 0);
  return NULL;
}

void* weak_function(void *args){
    GPIOUnexport(25);
    GPIOExport(25);
    GPIODirection(25, 0);
  while (1)
  {
    if (GPIORead(25)) 
    {
      weak_button = 1;
    }
    usleep(10000);
  }
    GPIOUnexport(25);
}


void* signal_function(void* weatherresult){
  struct weatherResult* weather = (struct weatherResult*) weatherresult;
  flag = 1;
  GPIOExport(HG);
  GPIOExport(HR);
  GPIODirection(HG, 1);
  GPIODirection(HR, 1);
  GPIOExport(C_R);
  GPIOExport(C_G);
  GPIODirection(C_R, 1);
  GPIODirection(C_G, 1);
  GPIOWrite(C_R, 1);
  GPIOWrite(HR, 1);
  while (1){
    // if condition -> call proper function 
    int _weak_button = weak_button;
    if (_weak_button){
      printf("weak_button\n");
      int _flag = flag;
      if (_flag == 0){
        human(time_D * 1.5);  
        car(time_D);
      }
      else{
        car(time_D * 0.8);
        human(time_D * 1.5);  
        car(time_D);
      }
      weak_button = 0;
    }
    else if (weather -> rain){
      human((int)(time_D * 1.5));
      car ((int)(time_D * 1.2));
    }
    else if (weather -> DHT.humi > 80 && weather -> DHT.temp > 30){
      printf("bad weather\n");
      human((int)(time_D * 1.2));
      car ((int)(time_D * 1.2));
    }
    else if (weather-> light >= 800){
      printf("일조량이 너무 많습니다. 파라솔을 펼쳐 주세요.\n");
      human(time_D);  
      car(time_D);
    }
    else {
    printf("default signal\n");
    human(time_D);  
    car(time_D);
    }
  }

GPIOUnexport(HG);
GPIOUnexport(HR);
GPIOUnexport(C_R);
GPIOUnexport(C_G);
}

int main() 
{
  pthread_t pthread[3]; 
  int thr_id;
  int status;
  struct weatherResult *weatherresult = (struct weatherResult *)malloc(sizeof(struct weatherResult));

  thr_id = pthread_create(&pthread[0], NULL, weak_function, NULL);
  if (thr_id < 0) {
    perror("thread1 create error : ");
    exit(0);
  }
  thr_id = pthread_create(&pthread[1], NULL, signal_function, weatherresult);
  if (thr_id < 0) {
    perror("thread2 create error : ");
    exit(0);
  }
  thr_id = pthread_create(&pthread[2], NULL, weatherThread, weatherresult);
  if (thr_id < 0) {
    perror("thread3 create error : ");
    exit(0);
  }

  pthread_join(pthread[0], NULL);
  pthread_join(pthread[1], NULL);
  pthread_join(pthread[2], NULL);
  free(weatherresult);

  return 0;
}