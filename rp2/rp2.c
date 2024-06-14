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
    GPIOUnexport(21);
    GPIOExport(21);
    GPIODirection(21, 0);
  while (1)
  {
    if (GPIORead(21)) weak_button = 1;
    usleep(10);
  }
    GPIOUnexport(21);
}


void* signal_function(void* weatherresult){
  struct weatherResult* weather = (struct weatherResult*) weatherresult;

  int flag = 1;
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
    
    if (weak_button){
      printf("weak_button\n");
      if (flag == 0){
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
      printf("파라솔을 펼치시기 바랍니다 개더우니깐요\n");
      human(time_D);  
      car(time_D);
    }
    else {
    printf("else\n");
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