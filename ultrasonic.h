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
 * __us_read(@trig, @echo)
 * DESCRIPTION
 *  Read GPIO from ultrasonic sensor.
 *  Assuming @trig and @echo is set properly.
 * 
 * RETURN
 *  Measured distance(cm); -1 if error.
 */
double __us_read(int trig, int echo);

void __us_finalize(void *args);

/**
 * us_thread(@args)
 * DESCRIPTION
 *  Routine that reads value from the ultrasonic sensor.
 *  The type of @args is to be `struct __thread_args_us`.
 *  Get measured speed via `args->speed`.
 *  Note that this function do not return anything.
 */
void *us_thread(void *args);


#endif