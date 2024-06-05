#include <stdio.h>
#include <fcntl.h>
#include "pwm.h"

void PWMExport(int pwm_num)
{
    printf("[PWMExport] Exporting pwm %d\n", pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMExport] @pwm_num must be 0 or 1\n");
        return;
    }
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMExport] File open failed");
        return;
    }
    if (dprintf(fd, "%d", pwm_num) == -1)
    {
        perror("[PWMExport] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMExport] Exported pwm %d\n", pwm_num);
}

void PWMUnexport(int pwm_num)
{
    printf("[PWMUnexport] Unexporting pwm %d\n", pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMUnexport] @pwm_num must be 0 or 1\n");
        return;
    }
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMUnexport] File open failed");
        return;
    }
    if (dprintf(fd, "%d", pwm_num) == -1)
    {
        perror("[PWMUnexport] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMUnexport] Unexported pwm %d\n", pwm_num);
}

void PWMEnable(int pwm_num)
{
    printf("[PWMEnable] Enabling pwm %d\n", pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMEnable] @pwm_num must be 0 or 1\n");
        return;
    }
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num) == -1)
    {
        perror("[PWMEnable] Path write failed");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMEnable] File open failed");
        return;
    }
    if (dprintf(fd, "1") == -1)
    {
        perror("[PWMEnable] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMEnable] Enabled pwm %d\n", pwm_num);
}

void PWMDisable(int pwm_num)
{
    printf("[PWMDisable] Disabling pwm %d\n", pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMDisable] @pwm_num must be 0 or 1\n");
        return;
    }
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_ENABLE_PATH, pwm_num) == -1)
    {
        perror("[PWMDisable] Path write failed");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMDisable] File open failed");
        return;
    }
    if (dprintf(fd, "0") == -1)
    {
        perror("[PWMDisable] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMDisable] Disabled pwm %d\n", pwm_num);
}

void PWMWritePeriod(int pwm_num, int value)
{
    printf("[PWMWritePeriod] Writing period %d pwm %d\n", value, pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMWritePeriod] @pwm_num must be 0 or 1\n");
        return;
    }
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_PERIOD_PATH, pwm_num) == -1)
    {
        perror("[PWMWritePeriod] Path write failed");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMWritePeriod] File open failed");
        return;
    }
    if (dprintf(fd, "%d", value) == -1)
    {
        perror("[PWMWritePeriod] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMWritePeriod] Writed period %d pwm %d\n", value, pwm_num);
}

void PWMWriteDutyCycle(int pwm_num, int value)
{
    printf("[PWMWriteDutyCycle] Writing duty cycle %d pwm %d\n", value, pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMWriteDutyCycle] @pwm_num must be 0 or 1\n");
        return;
    }
    char buffer[BUFLEN];
    if (snprintf(buffer, BUFLEN, PWM_DUTYCYCLE_PATH, pwm_num) == -1)
    {
        perror("[PWMWriteDutyCycle] Path write failed");
        return;
    }
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        perror("[PWMWriteDutyCycle] File open failed");
        return;
    }
    if (dprintf(fd, "%d", value) == -1)
    {
        perror("[PWMWriteDutyCycle] Write failed");
        close(fd);
        return;
    }
    close(fd);
    printf("[PWMWriteDutyCycle] Writed duty cycle %d pwm %d\n", value, pwm_num);
}

void PWMWriteRatio(int pwm_num, float ratio)
{
    printf("[PWMWriteRatio] Writing ratio %.2f%% pwm %d\n", ratio * 100, pwm_num);
    if (pwm_num != 0 && pwm_num != 1)
    {
        fprintf(stderr, "[PWMWriteRatio] @pwm_num must be 0 or 1\n");
        return;
    }
    if (ratio < 0 || 1 < ratio)
    {
        fprintf(stderr, "[PWMWriteRatio] @ratio must be between 0 and 1\n");
        return;
    }
    PWMWritePeriod(pwm_num, DEFAULT_PERIOD);
    PWMWriteDutyCycle(pwm_num, (int)(ratio * DEFAULT_PERIOD));
    printf("[PWMWriteRatio] Writed ratio %.2f%% pwm %d\n", ratio * 100, pwm_num);
}