#ifndef __RP3_H__
#define __RP3_H__

#include <stdint.h>
#include <pthread.h>
#include "rp2.h"

typedef struct
{
    pthread_t tid;
    int sockfd;
    struct weatherResult rp2_data;
    pthread_mutex_t lock_rp1, lock_rp2, lock_executed;
    pthread_cond_t cond_rp1, cond_executed;
} rp3_thread_args;

/**
 * Receive 긴급차량 signal from rp1. Uses `args->lock1`, `args->cond`.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__rp1_thread(void *args);

/**
 * Receive weather data from rp2. Access `args->rp2_data` to get the data. Uses `args->lock2`
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__rp2_thread(void *args);

/**
 * Read some datas from `us_thread` and `__rp2_thread`.
 * Also, check `args->cond_rp1` to receive 긴급차량 signal.
 * When the condition meets, call `__cheolkeong_execute_thread`.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *monitor_thread(void *args);

/**
 * Drop the road!
 * Send 철컹 signal to rp4, wait for some seconds, turn on the motor to restore the road.
 * If already 철컹 executing in another process, kill it and redo execution so that restore process will be postponed.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *execute_thread(void *args);

#endif