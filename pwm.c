#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "pwm.h"

int pwm_export(pwm_num pwm_num)
{
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_export] open");
        return -1;
    }
    if (dprintf(fd, "%d", pwm_num) == -1)
    {
        close(fd);
        perror("[pwm_export] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_unexport(pwm_num pwm_num)
{
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_unexport] open");
        return -1;
    }
    if (dprintf(fd, "%d", pwm_num) == -1)
    {
        close(fd);
        perror("[pwm_unexport] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_enable(pwm_num pwm_num)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_enable] open");
        return -1;
    }
    if (dprintf(fd, "1") == -1)
    {
        close(fd);
        perror("[pwm_enable] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_disable(pwm_num pwm_num)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_disable] open");
        return -1;
    }
    if (dprintf(fd, "0") == -1)
    {
        close(fd);
        perror("[pwm_disable] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_write_period(pwm_num pwm_num, uint32_t period)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_PERIOD_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_write_period] open");
        return -1;
    }
    if (dprintf(fd, "%d", period) == -1)
    {
        close(fd);
        perror("[pwm_write_period] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_write_duty_cycle(pwm_num pwm_num, uint32_t duty_cycle)
{
    char buffer[BUFLEN];
    snprintf(buffer, BUFLEN, PWM_DUTY_CYCLE_PATH, pwm_num);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[pwm_write_duty_cycle] open");
        return -1;
    }
    if (dprintf(fd, "%d", duty_cycle) == -1)
    {
        close(fd);
        perror("[pwm_write_duty_cycle] dprintf");
        return -1;
    }
    close(fd);
    return 0;
}

int pwm_ratio(pwm_num pwm_num, float ratio)
{
    if (ratio < 0 || 1 < ratio)
    {
        fprintf(stderr, "[pwm_ratio] Invalid ratio\n");
        return -1;
    }
    if (pwm_write_period(pwm_num, DEFAULT_PERIOD) == -1)
        return -1;
    unsigned int __duty_cycle = (int)(DEFAULT_PERIOD * ratio);
    if (pwm_write_duty_cycle(pwm_num, __duty_cycle) == -1)
        return -1;
    return 0;
}