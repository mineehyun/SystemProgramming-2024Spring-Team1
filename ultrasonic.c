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
    gpio_value value;
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
    us_thread_args *__args = (us_thread_args *)args;
    gpio_unexport(__args->trig);
    gpio_unexport(__args->echo);
    free(__args);
}

void *us_thread(void *args)
{
    us_thread_args *__args = (us_thread_args *)args;
    /* Init GPIO */
    if (gpio_export(__args->trig) ||
        gpio_export(__args->echo) ||
        gpio_set_direction(__args->trig, OUT) ||
        gpio_set_direction(__args->echo, IN))
    {
        __args->speed = -1;
        perror("[us_thread] GPIO");
        return NULL;
    }
    /* Start */
    clock_t start, end, dt;
    double distance_old, distance = 0, speed;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(__us_thread_finalize, args);
    while (1)
    {
        start = clock();
        usleep(1000000 / __args->polling_rate);
        distance_old = distance;
        distance = __us_read(__args->trig, __args->echo);
        end = clock();
        speed = (distance_old - distance) / ((double)(end - start) / CLOCKS_PER_SEC);
        if (speed < 0 || 3000 < speed)
            continue;
        __args->speed = speed;
    }
    pthread_cleanup_pop(0);
}