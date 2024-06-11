#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include "rp2.h"
#include "rain.c"
#include "dht11.c"
#include "light.c"
#include "socket.c"

#define getWeatherTimeGap 5
void* weatherThread(void* __weatherresult) {
  struct weatherResult* weatherresult = (struct weatherResult*)__weatherresult;
  int sock_addr = socket_client("192.168.14.10", 8882);
  int cnt = 0;
  pthread_t p_thread[3];
  int thr_id;
  int light;
  int rain;
  struct DHTresult dhtResult;
  // initialize dht  
  dhtResult.humi = 50;
  dhtResult.temp = 20;

  while(1){
    thr_id = pthread_create(&p_thread[0], NULL, (void*)light_function, &light);
    if (thr_id < 0) {
      perror("thread1 create error : ");
      exit(0);
    }
    thr_id = pthread_create(&p_thread[1], NULL, (void*)rain_function, &rain);
    if (thr_id < 0) {
      perror("thread2 create error : ");
      exit(0);
    }
    thr_id = pthread_create(&p_thread[2], NULL, (void*)dht_function, &dhtResult);
    if (thr_id < 0) {
      perror("thread3 create error : ");
      exit(0);
    }
    pthread_join(p_thread[0], NULL);
    pthread_join(p_thread[1], NULL);
    pthread_join(p_thread[2], NULL);

    weatherresult -> DHT = dhtResult;
    weatherresult -> light = light;
    weatherresult -> rain = rain;

    // event function들 return값 정리 -> 날씨 저장
    printf("DHTresult: %d, %d\n", weatherresult -> DHT.humi, weatherresult -> DHT.temp);
    printf("light: %d\n", weatherresult -> light);
    printf("rain: %d\n", weatherresult -> rain);
    sleep(getWeatherTimeGap);
    
    if (getWeatherTimeGap * cnt == 10){ // 30초마다 한 번씩
      write(sock_addr, weatherresult, sizeof(struct weatherResult));
      cnt = 0;
    }
    else cnt++; 
    }
}