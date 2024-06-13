#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "rgb_led.h"
#include "../gpio.h"

#define RED 17
#define GREEN 27
#define BLUE 22

extern int gas_level;
extern int gas_threshold;
extern pthread_mutex_t lock;

void *rgb_led_function(void *vargp)
{
    gpio_export(RED);
    gpio_export(GREEN);
    gpio_export(BLUE);
    gpio_set_direction(RED, OUT);
    gpio_set_direction(GREEN, OUT);
    gpio_set_direction(BLUE, OUT);
    int local_gas_level;
    while (1)
    {
        pthread_mutex_lock(&lock);
        local_gas_level = gas_level;
        pthread_mutex_unlock(&lock);
        if (local_gas_level > gas_threshold)
        {
            // Flash yellow rapidly
            gpio_write(RED, 1);
            gpio_write(GREEN, 1);
            usleep(250000); // 0.25 seconds on
            gpio_write(RED, 0);
            gpio_write(GREEN, 0);
            gpio_write(BLUE, 0);
            usleep(250000); // 0.25 seconds off
        }
        else
        {
            // Flash red and blue alternately
            gpio_write(RED, 1);
            usleep(500000); // 0.5 seconds
            gpio_write(RED, 0);
            gpio_write(BLUE, 1);
            usleep(500000); // 0.5 seconds
            gpio_write(BLUE, 0);
        }
        usleep(100000); // 0.1 seconds delay to prevent CPU overload
    }
    gpio_unexport(RED);
    gpio_unexport(GREEN);
    gpio_unexport(BLUE);
}
