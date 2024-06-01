#include <stdio.h>
#include <fcntl.h>
#include "pwm.h"

int PWMExport(int pwm_num)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", pwm_num) == -1) return -1;
    close(fd);
    return 0;
}

int PWMUnexport(int pwm_num)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", pwm_num) == -1) return -1;
    close(fd);
    return 0;
}

int PWMEnable(int pwm_num)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (write(fd, "1", 2) == -1) return -1;
    close(fd);
    return 0;
}

int PWMDisable(int pwm_num)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (write(fd, "0", 2) == -1) return -1;
    close(fd);
    return 0;
}

int PWMWritePeriod(int pwm_num, int value)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_PERIOD_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", value) == -1) return -1;
    close(fd);
    return 0;
}

int PWMWriteDutyCycle(int pwm_num, int value)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_DUTYCYCLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1) return -1;
    if (dprintf(fd, "%d", value) == -1) return -1;
    close(fd);
    return 0;
}

int PWMWriteDutyCycleByRatio(int pwm_num, float ratio)
{
    if (pwm_num != 0 && pwm_num != 1) return -1;
    if (ratio < 0|| ratio > 1) return -1;
    if (PWMWritePeriod(pwm_num, DEFAULT_PERIOD)) return -1;
    if (PWMWriteDutyCycle(pwm_num, DEFAULT_PERIOD * ratio)) return -1;
    return 0;
}