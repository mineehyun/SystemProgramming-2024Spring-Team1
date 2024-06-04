#include <stdio.h>
#include <fcntl.h>
#include "gpio.h"

void GPIOExport(int pin)
{
    int fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
        perror("[GPIOExport] file open failed");
    if (dprintf(fd, "%d", pin) == -1)
        perror("[GPIOExport] dprintf failed");
    close(fd);
}

void GPIOUnexport(int pin)
{
    int fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
        perror("[GPIOUnexport] file open failed");
    if (dprintf(fd, "%d", pin) == -1)
        perror("[GPIOUnexport] dprintf failed");
    close(fd);
}

void GPIODirection(int pin, int dir)
{
    /* @dir exception */
    if (dir != GPIO_IN && dir != GPIO_OUT)
    {
        fprintf(stderr, "[GPIODirection] @dir must be GPIO_IN(0) or GPIO_OUT(1)\n");
        return;
    }
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_DIRECTION_PATH, pin) == -1)
        perror("[GPIODirection] snprintf failed");
    /* Open file */
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
        perror("[GPIODirection] file open failed");
    /* Write */
    char *dir_strs[2] = { "in", "out" };
    if (dprintf(fd, dir_strs[dir]) == -1)
        perror("[GPIODirection] dprintf failed");
    /* Finalize */
    close(fd);
}

int GPIORead(int pin)
{
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
        perror("[GPIORead] snprintf failed");
    /* Open file */
    int fd = open(buffer, O_RDONLY);
    if (fd == -1)
        perror("[GPIORead] file open failed");
    /* Read */
    if (read(fd, buffer, 3) == -1)
        perror("[GPIORead] read failed");
    /* Finalize */
    close(fd);
    return atoi(buffer);
}

void GPIOWrite(int pin, int value)
{
    /* @value exception */
    if (value != GPIO_LOW && value != GPIO_HIGH)
    {
        fprintf(stderr, "[GPIODirection] @value must be GPIO_LOW(0) or GPIO_HIGH(1)\n");
        return;
    }
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
        perror("[GPIOWrite] snprintf failed");
    /* Open file */
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
        perror("[GPIOWrite] file open failed");
    /* Write */
    if (dprintf(fd, "%d", value) == -1)
        perror("[GPIOWrite] dprintf failed");
    /* Finalize */
    close(fd);
}