#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "../gpio.h"
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
    double distance = dt * 34300 / 2;
    if (distance < 0 || 100 < distance)
    {
        distance = -1;
    }
    return distance;
}

void __us_thread_finalize(void *args)
{
    us_thread_args *__args = (us_thread_args *)args;
    gpio_unexport(__args->trig);
    gpio_unexport(__args->echo);
}

void *us_thread(void *args)
{
    us_thread_args *__args = (us_thread_args *)args;
    /* Init GPIO */
    gpio_export(__args->trig);
    gpio_export(__args->echo);
    pthread_cleanup_push(__us_thread_finalize, args);
    gpio_set_direction(__args->trig, OUT);
    gpio_set_direction(__args->echo, IN);
    /* Start */
    clock_t start, end, dt;
    double distance_old, distance = 0, speed;
    while (1)
    {
        start = clock();
        usleep(DELTA_T);
        distance_old = distance;
        distance = __us_read(__args->trig, __args->echo);
        /* if distance is invalid */
        if (distance == -1)
        {
            /* Restore value for next iteration */
            distance = distance_old;
            /* Update speed to 0 to assume there is no car */
            speed = 0.0;
        }
        else
        {
            end = clock();
            speed = (distance_old - distance) / ((double)(end - start) / CLOCKS_PER_SEC);
        }
        /* Update speed in lock */
        pthread_mutex_lock(&__args->lock);
        __args->speed = speed;
        pthread_mutex_unlock(&__args->lock);
    }
    pthread_cleanup_pop(0);
}