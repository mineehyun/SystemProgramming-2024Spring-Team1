#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include <pthread.h>

typedef struct
{
    pthread_mutex_t lock;
    int polling_rate;
    int trig, echo;
    double speed;
} us_thread_args;

/**
 * Read ultrasonic sensor. Do not call manually.
 *
 * @returns
 * Measured distance(cm). -1 if error.
 */
double __us_read(int trig, int echo);

/**
 * Thread finalize function of `us_thread`.
 *
 * @param args
 * Use type `us_thread_args *`.
 */
void __us_thread_finalize(void *args);

/**
 * Read ultrasonic sensor repeatedly. Access `args->speed` to get measured speed. It will be -1 if error.
 *
 * @param args
 * Use type `us_thread_args *`.
 */
void *us_thread(void *args);

#endif