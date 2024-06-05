#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include <pthread.h>

typedef struct __thread_args_us
{
    pthread_t *tid;
    int polling_rate;
    int trig, echo;
    double *speed;
} targs_us;

/**
 * @brief
 * Read GPIO from ultrasonic sensor.
 * 
 * @return
 * Measured distance(cm); -1 if error.
 */
double __us_read(int trig, int echo);

void __us_finalize(void *args);

/**
 * @brief
 * Routine that reads value from the ultrasonic sensor.
 * Access measured speed via `args->speed`.
 * 
 * @param args
 * Use type `struct __thread_args_us *`.
 * 
 * @return
 * Nothing.
 */
void *us_thread(void *args);

#endif