#include <stdio.h>
#include <fcntl.h>
#include "gpio.h"

void GPIOExport(int pin)
{
    int fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOExport] open: ");
    }
    else if (dprintf(fd, "%d", pin) == -1)
    {
        perror("[GPIOExport] dprintf: ");
    }
    close(fd);
}

void GPIOUnexport(int pin)
{
    int fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOUnexport] open: ");
    }
    else if (dprintf(fd, "%d", pin) == -1)
    {
        perror("[GPIOUnexport] dprintf: ");
    }
    close(fd);
}

void GPIODirection(int pin, gpio_direction direction)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_DIRECTION_PATH, pin) == -1)
    {
        perror("[GPIODirection] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    char *dir_strs[2] = { "in", "out" };
    if (fd == -1)
    {
        perror("[GPIODirection] open: ");
    }
    else if (dprintf(fd, dir_strs[direction]) == -1)
    {
        perror("[GPIODirection] dprintf: ");
    }
    close(fd);
}

void GPIOWrite(int pin, gpio_status status)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
    {
        perror("[GPIOWrite] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOWrite] open: ");
    }
    else if (dprintf(fd, "%d", status) == -1)
    {
        perror("[GPIOWrite] dprintf: ");
    }
    close(fd);
}

gpio_status GPIORead(int pin)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
    {
        perror("[GPIORead] snprintf: ");
        return -1;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIORead] open: ");
        return -1;
    }
    if (read(fd, buffer, 3) == -1)
    {
        perror("[GPIORead] read: ");
        close(fd);
        return -1;
    }
    close(fd);
    return atoi(buffer);
}