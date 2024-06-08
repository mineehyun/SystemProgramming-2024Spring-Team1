#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "gpio.h"
#include "ultrasonic.h"

double __us_read(int trig, int echo)
{
    /* Make a pulse */
    gpio_write(trig, HIGH);
    usleep(10);
    gpio_write(trig, LOW);
    /* Recieve pulse */
    clock_t start, end;
    enum gpio_value value;
    do
    {
        gpio_read(echo, &value);
        start = clock();
    } while (value == LOW);
    do
    {
        gpio_read(echo, &value);
        end = clock();
    } while (value == HIGH);
    /* Calc distance(cm) */
    double dt = (double)(end - start) / CLOCKS_PER_SEC;
    return dt * 34300 / 2;
}

void __us_thread_finalize(void *args)
{
    struct us_thread_args *__args = (struct us_thread_args *)args;
    gpio_unexport(__args->trig);
    gpio_unexport(__args->echo);
    free(__args);
}

void *us_thread(void *args)
{
    struct us_thread_args *__args = (struct us_thread_args *)args;
    /* Init GPIO */
    gpio_export(__args->trig);
    gpio_export(__args->echo);
    gpio_set_direction(__args->trig, OUT);
    gpio_set_direction(__args->echo, IN);
    /* Start */
    clock_t start, end, dt;
    double distance_old, distance = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(__us_thread_finalize, args);
    while (1)
    {
        start = clock();
        usleep(1000000 / __args->polling_rate);
        distance_old = distance;
        distance = __us_read(__args->trig, __args->echo);
        end = clock();
        __args->speed = (distance_old - distance) / ((double)(end - start) / CLOCKS_PER_SEC);
    }
    pthread_cleanup_pop(0);
}