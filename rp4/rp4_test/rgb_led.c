#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "rgb_led.h"

#define RED 17
#define GREEN 27
#define BLUE 22 

extern int gas_level;
extern pthread_mutex_t lock;

void gpio_export(int pin) {
    char buffer[3];
    int len;
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        perror("gpio/export");
        return;
    }
    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    write(fd, buffer, len);
    close(fd);
}

void gpio_unexport(int pin) {
    char buffer[3];
    int len;
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        perror("gpio/unexport");
        return;
    }
    len = snprintf(buffer, sizeof(buffer), "%d", pin);
    write(fd, buffer, len);
    close(fd);
}

void gpio_direction(int pin, const char* dir) {
    char path[35];
    int fd;
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("gpio/direction");
        return;
    }
    write(fd, dir, strlen(dir));
    close(fd);
}

void gpio_write(int pin, int value) {
    char path[35];
    int fd;
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("gpio/set-value");
        return;
    }
    if (value)
        write(fd, "1", 1);
    else
        write(fd, "0", 1);
    close(fd);
}

void turn_on(const char* color) {
    if (strcmp(color, "RED") == 0) {
        gpio_write(RED, 1);
        sleep(1);
        gpio_write(RED, 0);
    } else if (strcmp(color, "GREEN") == 0) {
        gpio_write(GREEN, 1);
        sleep(1);
        gpio_write(GREEN, 0);
    } else if (strcmp(color, "BLUE") == 0) {
        gpio_write(BLUE, 1);
        sleep(1);
        gpio_write(BLUE, 0);
    }
}

void turn_off_all() {
    gpio_write(RED, 0);
    gpio_write(GREEN, 0);
    gpio_write(BLUE, 0);
}

void *rgb_led_function(void *vargp) {
    gpio_export(RED);
    gpio_export(GREEN);
    gpio_export(BLUE);
    gpio_export(12); //e

    gpio_direction(RED, "out");
    gpio_direction(GREEN, "out");
    gpio_direction(BLUE, "out");
    gpio_direction(12, "out");

    while (1) {
        pthread_mutex_lock(&lock);
        int current_gas_level = gas_level;
        pthread_mutex_unlock(&lock);

        if (current_gas_level > 300) {
            gpio_write(12,1); //e
            // Flash yellow rapidly
            for (int i = 0; i < 5; i++) {  // Flash yellow 5 times
                gpio_write(RED, 1);
                gpio_write(GREEN, 1);
                usleep(250000);  // 0.25 seconds on
                turn_off_all();
                usleep(250000);  // 0.25 seconds off

                pthread_mutex_lock(&lock);
                if (gas_level <= 300) {
                    //gpio_write(12,0); //e
                    pthread_mutex_unlock(&lock);
                    gpio_write(12,0); //e
                    //gpio_unexport(12);
                    break;  // Exit if gas level goes back to normal
                }
                pthread_mutex_unlock(&lock);
            }
        } else {
            // Flash red and blue alternately
            gpio_write(RED, 1);
            usleep(500000);  // 0.5 seconds
            gpio_write(RED, 0);
            gpio_write(BLUE, 1);
            usleep(500000);  // 0.5 seconds
            gpio_write(BLUE, 0);
        }

        usleep(100000);  // 0.1 seconds delay to prevent CPU overload
    }

    gpio_unexport(RED);
    gpio_unexport(GREEN);
    gpio_unexport(BLUE);
    gpio_unexport(12); //e

    return NULL;
}