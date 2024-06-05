#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "gpio.h"
#include "ultrasonic.h"

extern void GPIOExport(int pin);
extern void GPIOUnexport(int pin);
extern void GPIODirection(int pin, int dir);
extern void GPIOWrite(int pin, int value);
extern int GPIORead(int pin);

double __us_read(int trig, int echo)
{
    clock_t ini_t, fin_t;
    /* Make a pulse */
    GPIOWrite(trig, 1);
    usleep(10);
    GPIOWrite(trig, 0);
    /* Recieve pulse */
    while (GPIORead(echo) == 0) ini_t = clock();
    while (GPIORead(echo) == 1) fin_t = clock();
    /* Calculate distance */
    double dt = (double)(fin_t - ini_t) / CLOCKS_PER_SEC;
    return dt * 34300 / 2;
}

void __us_finalize(void *args)
{
    targs_us *temp = (targs_us *)args;
    GPIOUnexport(temp->trig);
    GPIOUnexport(temp->echo);
    free(temp->tid);
    free(temp->speed);
    free(temp);
}

void *us_thread(void *args)
{
    targs_us *temp = (targs_us *)args;
    /* Init GPIO */
    GPIOExport(temp->trig);
    GPIOExport(temp->echo);
    GPIODirection(temp->trig, OUT);
    GPIODirection(temp->echo, IN);
    /* Start */
    clock_t ini_t, fin_t, dt;
    double distance_old, distance = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(__us_finalize, args);
    while (1)
    {
        ini_t = clock();
        distance_old = distance;
        dt = CLOCKS_PER_SEC / temp->polling_rate;
        while (fin_t > ini_t + dt) fin_t = clock();
        distance = __us_read(temp->trig, temp->echo);
        *temp->speed = (distance_old - distance) / ((fin_t - ini_t) / CLOCKS_PER_SEC);
    }
    pthread_cleanup_pop(0);
}