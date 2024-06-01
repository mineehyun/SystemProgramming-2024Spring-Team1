#include <unistd.h>
#include <time.h>
#include "ultrasonic.h"

int __us_ready(int trig, int echo)
{
    if (GPIOExport(trig)) return -1;
    if (GPIOExport(echo)) return -1;
    if (GPIODirection(trig, GPIO_OUT)) return -1;
    if (GPIODirection(echo, GPIO_IN)) return -1;
    return 0;
}

double __us_read(int trig, int echo)
{
    clock_t ini_t, fin_t;
    /* Make a pulse */
    if (GPIOWrite(trig, 1)) return -1;
    usleep(10);
    if (GPIOWrite(trig, 0)) return -1;
    /* Recieve pulse */
    while (GPIORead(echo) == 0) ini_t = clock();
    while (GPIORead(echo) == 1) fin_t = clock();
    /* Calculate delta t */
    double dt = (double)(fin_t - ini_t) / CLOCKS_PER_SEC;
    return dt * 34300 / 2;
}

int __us_close(int trig, int echo)
{
    if (GPIOUnexport(trig)) return -1;
    if (GPIOUnexport(echo)) return -1;
    return 0;
}

void __us_thread_finalize(void *args)
{
    targs_us *temp = (targs_us *)args;
    __us_close(temp->trig, temp->echo);
    free(temp->tid);
    free(temp->speed);
    free(temp);
}

void *us_thread(void *args)
{
    targs_us *temp = (targs_us *)args;
    int polling_rate = temp->polling_rate;
    int trig = temp->trig;
    int echo = temp->echo;
    int *speed = temp->speed;
    __us_ready(trig, echo);
    clock_t ini_t, fin_t, d_clock;
    double distance_old, distance = __us_read(trig, echo);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(__us_thread_finalize, args);
    while (1)
    {
        ini_t = clock();
        distance_old = distance;
        d_clock = CLOCKS_PER_SEC / polling_rate;
        while (fin_t > ini_t + d_clock) fin_t = clock();
        distance = __us_read(trig, echo);
        *speed = (distance_old - distance) / ((fin_t - ini_t) / CLOCKS_PER_SEC);
    }
    pthread_cleanup_pop(0);
}