#ifndef __RP3_H__
#define __RP3_H__

#include <stdint.h>
#include <pthread.h>
#include "../rp2/rp2.h"

/* Speed threshold configs */
#define SPEED_THRESHOLD_DEFAULT 1000
#define SPEED_THRESHOLD_WET_DECREASE_RATIO 0.8 // if wet, threshold *= 0.8
#define SPEED_THRESHOLD_FOG_DECREASE_RATIO 0.7 // if fog, threshold *= 0.7
#define LIGHT_THRESHOLD 250 // determine fog state
#define HUMID_THRESHOLD 50 // determine fog state

/* Duration to activate motor */
#define CHOELKEONG_DURATION 10 // 철컹 지속시간(초)
#define MOTOR_DURATION 10 // 모터 지속시간(초)
#define TEST_CANCEL_INTERVAL 100000 // check cancel every 0.1s

/* Monitoring interval */
#define MONITOR_INTERVAL 100000 // check 철컹 condition every 0.1s
#define SOCKET_INTERVAL 100000 // read socket every 0.1s

#ifndef BUFLEN
#define BUFLEN 256
#endif

typedef struct
{
    /* Some mutexes for `__execute_thread` */
    pthread_t tid_executing;
    pthread_mutex_t lock_tid_executing, lock_motor;
    /* data from rp2 */
    struct weatherResult rp2_data;
    pthread_mutex_t lock_rp2_data;
    /* data from ultrasonic sensor */
    us_thread_args *us_data;
    /* other thread configs */
    int sockfd_rp1, sockfd_rp2, sockfd_rp4;
    uint8_t cheolkeong_pin; // 이 핀 1로 설정하면 철컹함
    uint8_t motor_pin; // 이 핀 1로 설정하면 모터 돌아서 도로 원상복구함
} rp3_thread_args;

/**
 * Receive 긴급차량 signal from rp1.
 * 뭐라도받으면철컹해버립니다조심하세요.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__rp1_thread(void *args);

/**
 * Receive weather data from rp2.
 * Access `args->rp2_data` to get the data. Use `args->lock_rp2_data`.
 * 오류테스트안합니다멀쩡한데이터보내세요.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__rp2_thread(void *args);

/**
 * Thread finalize function of `monitor_thread`.
 *
 * @param args
 * Use type `rp3_thread_args *`.
 */
void __monitor_thread_finalize(void *args);

/**
 * Read speed from `us_thread` and read weather from `__rp2_thread`.
 * When the condition meets, call `__execute_thread`.
 * If any data is -1, terminate thread.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *monitor_thread(void *args);

/**
 * Thread finalize function of `__execute_thread`.
 *
 * @param args
 * Use type `rp3_thread_args *`.
 */
void __execute_thread_finalize(void *args);

/**
 * 철컹해버림. 긴급차량 신호나 모니터 스레드에서 호출됨.
 * 한 번에 하나의 스레드만 진입 가능함을 보장함.
 * 이미 함수 진행 중인데 다른 스레드 들어오면 진행 중인 스레드 캔슬해버림.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__execute_thread(void *args);

#endif