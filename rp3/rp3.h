#ifndef __RP3_H__
#define __RP3_H__

#include <stdint.h>
#include <pthread.h>
#include "rp2.h"

/* Speed threshold configs */
#define SPEED_THRESHOLD_DEFAULT 1000
#define SPEED_THRESHOLD_WET_DECREASE_RATIO 0.8 // if wet, threshold *= 0.8
#define SPEED_THRESHOLD_FOG_DECREASE_RATIO 0.7 // if fog, threshold *= 0.7
#define LIGHT_THRESHOLD 250 // determine fog state
#define HUMID_THRESHOLD 0.5 // determine fog state

/* Duration to activate motor */
#define MOTOR_DURATION 10 // sec

#ifndef BUFLEN
#define BUFLEN 256
#endif

typedef struct
{
    pthread_t tid_executing;
    int sockfd_rp1;
    struct weatherResult rp2_data;
    us_thread_args *us_data;
    pthread_mutex_t lock_rp2, lock_execute;
    pthread_cond_t cond_executed;
    int polling_rate;
    uint8_t motor_pin;
} rp3_thread_args;

/**
 * Thread finalize function of `__rp1_thread`.
 *
 * @param args
 * Use type `rp3_thread_args *`.
 */
void __rp1_thread_finalize(void *args);

/**
 * Receive 긴급차량 signal from rp1.
 * If 1 accepted, executes `execute_thread`.
 * Otherwise, terminate thread.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *__rp1_thread(void *args);

/**
 * Thread finalize function of `__rp2_thread`.
 *
 * @param args
 * Use type `rp3_thread_args *`.
 */
void __rp2_thread_finalize(void *args);

/**
 * Receive weather data from rp2.
 * Access `args->rp2_data` to get the data. Use `args->lock_rp2`.
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
 * When the condition meets, call `__cheolkeong_execute_thread`.
 * If any data is -1, terminate thread.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *monitor_thread(void *args);

/**
 * Thread finalize function of `execute_thread`.
 *
 * @param args
 * Use type `rp3_thread_args *`.
 */
void __execute_thread_finalize(void *args);

/**
 * Drop the road!
 * Send 철컹 signal to rp4, wait for some seconds, turn on the motor to restore the road.
 * If already 철컹 executing in another process,
 * cancel it and redo execution to postpone restore process.
 * 
 * @param args
 * Use type `rp3_thread_args *`.
 */
void *execute_thread(void *args);

#endif