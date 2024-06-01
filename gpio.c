#include <stdio.h>
#include <fcntl.h>
#include "gpio.h"

int GPIOExport(int pin_num)
{
    int fd;
    fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", pin_num) == -1) return -1;
    close(fd);
    return 0;
}

int GPIOUnexport(int pin_num)
{
    int fd;
    fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", pin_num) == -1) return -1;
    close(fd);
    return 0;
}

int GPIODirection(int pin_num, int direction)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_DIRECTION_PATH, pin_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (direction == GPIO_IN)
    {
        if (write(fd, "in", 3) == -1) return -1;
    }
    else if (direction == GPIO_OUT)
    {
        if (write(fd, "out", 4) == -1) return -1;
    }
    else return -1;
    return 0;
}

int GPIORead(int pin_num)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin_num);
    int fd = open(buffer, O_RDONLY);
    if (fd == -1) return -1;
    if (read(fd, buffer, 3) == -1) return -1;
    return atoi(buffer);
}

int GPIOWrite(int pin_num, int value)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (value == 0)
    {
        if (write(fd, "0", 2) == -1) return -1;
    }
    else if (value == 1)
    {
        if (write(fd, "1", 2) == -1) return -1;
    }
    else return -1;
    return 0;
}