#include <stdio.h>
#include <fcntl.h>
#include "gpio.h"

void GPIOExport(int pin)
{
    fprintf(stderr, "[GPIOExport] Exporting pin %d\n", pin);
    int fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOExport] File open failed");
        return;
    }
    if (dprintf(fd, "%d", pin) == -1)
    {
        perror("[GPIOExport] Write failed");
        close(fd);
        return;
    }
    close(fd);
    fprintf(stderr, "[GPIOExport] Exported pin %d\n", pin);
}

void GPIOUnexport(int pin)
{
    fprintf(stderr, "[GPIOExport] Exporting pin %d\n", pin);
    int fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOExport] File open failed");
        return;
    }
    if (dprintf(fd, "%d", pin) == -1)
    {
        perror("[GPIOExport] Write failed");
        close(fd);
        return;
    }
    close(fd);
    fprintf(stderr, "[GPIOExport] Exported pin %d\n", pin);
}

void GPIODirection(int pin, int dir)
{
    fprintf(stderr, "[GPIODirection] Directing pin %d\n", pin);
    /* @dir exception */
    if (dir != IN && dir != OUT)
    {
        fprintf(stderr, "[GPIODirection] @dir must be IN(0) or OUT(1)\n");
        return;
    }
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_DIRECTION_PATH, pin) == -1)
    {
        perror("[GPIODirection] Path write failed");
        return;
    }
    /* Open file */
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIODirection] File open failed");
        return;
    }
    /* Write */
    char *dir_strs[2] = { "in", "out" };
    if (dprintf(fd, dir_strs[dir]) == -1)
    {
        perror("[GPIODirection] Write failed");
        close(fd);
        return;
    }
    /* Finalize */
    close(fd);
    fprintf(stderr, "[GPIODirection] Directed pin %d\n", pin);
}

void GPIOWrite(int pin, int value)
{
    fprintf(stderr, "[GPIOWrite] Writing value %d pin %d\n", value, pin);
    /* @value exception */
    if (value != HIGH && value != LOW)
    {
        fprintf(stderr, "[GPIOWrite] @value must be LOW(0) or HIGH(1)\n");
        return;
    }
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
    {
        perror("[GPIOWrite] Path write failed");
        return;
    }
    /* Open file */
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIOWrite] File open failed");
        return;
    }
    /* Write */
    if (dprintf(fd, "%d", value) == -1)
    {
        perror("[GPIOWrite] Write failed");
        close(fd);
        return;
    }
    /* Finalize */
    close(fd);
    fprintf(stderr, "[GPIOWrite] Writed value %d pin %d\n", value, pin);
}

int GPIORead(int pin)
{
    fprintf(stderr, "[GPIORead] Reading pin %d\n", pin);
    /* Set path */
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, GPIO_VALUE_PATH, pin) == -1)
    {
        perror("[GPIORead] Path write failed");
        return -1;
    }
    /* Open file */
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[GPIORead] File open failed");
        return -1;
    }
    /* Read */
    if (read(fd, buffer, 3) == -1)
    {
        perror("[GPIORead] Read failed");
        close(fd);
        return -1;
    }
    /* Finalize */
    close(fd);
    fprintf(stderr, "[GPIORead] Read pin %d value %s\n", pin, buffer);
    return atoi(buffer);
}