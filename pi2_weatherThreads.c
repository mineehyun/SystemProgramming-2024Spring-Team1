#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include "rain.c"
#include "dht11.c"
#include "light.c"


struct weatherResult {
  struct DHTresult DHT;
  char light; 
  int rain;
};

struct weatherResult* weatherresult;
int main() {
  pthread_t p_thread[3];
  int thr_id;
  int status;
  int* light = (int*)malloc(sizeof(int*));
  int* rain = (int*)malloc(sizeof(int*));
  char pM[] = "thread_m";
  struct DHTresult* dhtResult = (struct DHTresult*) malloc (sizeof(struct DHTresult*)); // dht 함수 나오는 거 봐서 포인터로 바꿔야 할 sudo
  weatherresult = (struct weatherResult*)malloc(sizeof(struct weatherResult*));

  for (int i =0; i<10; i++){
  thr_id = pthread_create(&p_thread[0], NULL, (void*)light_function, light);
  if (thr_id < 0) {
    perror("thread1 create error : ");
    exit(0);
  }
  thr_id = pthread_create(&p_thread[1], NULL, (void*)rain_function, rain);
  if (thr_id < 0) {
    perror("thread2 create error : ");
    exit(0);
  }
  thr_id = pthread_create(&p_thread[2], NULL, (void*)dht_function, dhtResult);
  if (thr_id < 0) {
    perror("thread3 create error : ");
    exit(0);
  }
  pthread_join(p_thread[0], (void **)&status);
  pthread_join(p_thread[1], (void **)&status);
  pthread_join(p_thread[2], (void **)&status);
  weatherresult -> DHT = *dhtResult;
  weatherresult -> light = *light;
  weatherresult -> rain = *rain;

  sleep(1);
  // event function들 return값 정리 -> 날씨 저장
  }
 
  return weatherresult;
}