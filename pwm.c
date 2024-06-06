#include <stdio.h>
#include <fcntl.h>
#include "pwm.h"

void PWMExport(int pwm_num)
{
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMExport] open: ");
    }
    else if (dprintf(fd, "%d", pwm_num) == -1)
    {
        perror("[PWMExport] dprintf: ");
    }
    close(fd);
}

void PWMUnexport(int pwm_num)
{
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMUnexport] open: ");
    }
    else if (dprintf(fd, "%d", pwm_num) == -1)
    {
        perror("[PWMUnexport] dprintf: ");
    }
    close(fd);
}

void PWMEnable(int pwm_num)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num) == -1)
    {
        perror("[PWMEnable] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMEnable] open: ");
    }
    else if (dprintf(fd, "1") == -1)
    {
        perror("[PWMEnable] dprintf: ");
    }
    close(fd);
}

void PWMDisable(int pwm_num)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num) == -1)
    {
        perror("[PWMDisable] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMDisable] open: ");
    }
    else if (dprintf(fd, "0") == -1)
    {
        perror("[PWMDisable] dprintf: ");
    }
    close(fd);
}

void PWMWritePeriod(int pwm_num, int value)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_PERIOD_PATH, pwm_num) == -1)
    {
        perror("[PWMPeriod] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMPeriod] open: ");
    }
    else if (dprintf(fd, "%d", value) == -1)
    {
        perror("[PWMPeriod] dprintf: ");
    }
    close(fd);
}

void PWMWriteDutyCycle(int pwm_num, int value)
{
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_DUTYCYCLE_PATH, pwm_num) == -1)
    {
        perror("[PWMDutyCycle] snprintf: ");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMDutyCycle] open: ");
    }
    else if (dprintf(fd, "%d", value) == -1)
    {
        perror("[PWMDutyCycle] dprintf: ");
    }
    close(fd);
}

void PWMWriteRatio(int pwm_num, float ratio)
{
    if (ratio < 0 || 1 < ratio)
    {
        fprintf(stderr, "[PWMRatio] Invalid ratio %.2f\n", ratio);
        return;
    }
    PWMWritePeriod(pwm_num, DEFAULT_PERIOD);
    PWMWriteDutyCycle(pwm_num, (int)(ratio * DEFAULT_PERIOD));
}