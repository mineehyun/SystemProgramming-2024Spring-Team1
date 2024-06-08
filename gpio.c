#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "gpio.h"

int gpio_export(int pin)
{
    int fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[gpio_export] open");
        return -1;
    }
    if (dprintf(fd, "%d", pin) == -1)
    {
        close(fd);
        perror("[gpio_export] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_unexport(int pin)
{
    int fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[gpio_unexport] open");
        return -1;
    }
    if (dprintf(fd, "%d", pin) == -1)
    {
        close(fd);
        perror("[gpio_unexport] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_set_direction(int pin, gpio_direction direction)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_DIRECTION_PATH, pin);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[gpio_set_direction] open");
        return -1;
    }
    if (dprintf(fd, (direction == IN) ? "in" : "out") == -1)
    {
        close(fd);
        perror("[gpio_set_direction] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_write(int pin, gpio_value value)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[gpio_write] open");
        return -1;
    }
    if (dprintf(fd, "%d", value) == -1)
    {
        close(fd);
        perror("[gpio_write] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_read(int pin, gpio_value *value)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin);
    int fd = open(buffer, O_RDONLY);
    if (fd == -1)
    {
        perror("[gpio_read] open");
        return -1;
    }
    if (read(fd, buffer, 3) == -1)
    {
        close(fd);
        perror("[gpio_read] read");
        return -1;
    }
    *value = atoi(buffer);
    close(fd);
    return 0;
}