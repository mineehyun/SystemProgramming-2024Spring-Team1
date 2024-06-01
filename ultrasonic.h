#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include <pthread.h>
#include "gpio.h"

extern int GPIOExport(int pin_num);
extern int GPIOUnexport(int pin_num);
extern int GPIODirection(int pin_num, int direction);
extern int GPIORead(int pin_num);
extern int GPIOWrite(int pin_num, int value);

/**
 * __us_ready(@trig, @echo)
 * Export GPIO and set its direction properly.
 * Call once before call `__us_read`.
 * 
 * RETURN
 * 0 : all GPIO are set properly.
 * Negative : GPIO error.
 */
int __us_ready(int trig, int echo);

/**
 * __us_read(@trig, @echo)
 * Read GPIO from ultrasonic sensor.
 * Assuming @trig and @echo is set properly.
 * May be called repeatedly.
 * Do not call directly in main routine; use `us_thread` instead.
 * 
 * RETURN
 * 측정된 거리(cm).
 * 음수: GPIO 에러.
 */
double __us_read(int trig, int echo);

int __us_close(int trig, int echo);

typedef struct __thread_args_us
{
    pthread_t *tid;
    int polling_rate;
    int trig, echo;
    double *speed;
} targs_us;

/**
 * __us_thread_finalize(@args)
 * Callback function called by `us_thread`. 
*/
void __us_thread_finalize(void *args);

/**
 * us_thread(@args)
 * Routine that reads value from ultrasonic sensor.
 * The type of @args is to be `struct __thread_args_us`.
 * Get measured speed value via `args->speed`.
 * This function do not return anything.
 */
void *us_thread(void *args);

#endif