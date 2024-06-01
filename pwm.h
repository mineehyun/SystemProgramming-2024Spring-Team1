#ifndef __PWM_H__
#define __PWM_H__

#define PWM_EXPORT_PATH "/sys/class/pwm/pwmchip0/export"
#define PWM_UNEXPORT_PATH "/sys/class/pwm/pwmchip0/unexport"
#define PWM_ENABLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/enable"
#define PWM_PERIOD_PATH "/sys/class/pwm/pwmchip0/pwm%d/period"
#define PWM_DUTYCYCLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle"

#define DEFAULT_PERIOD 100000

#ifndef BUFLEN
#define BUFLEN 256
#endif

int PWMExport(int pwm_num);
int PWMUnexport(int pwm_num);
int PWMEnable(int pwm_num);
int PWMDisable(int pwm_num);
int PWMWritePeriod(int pwm_num, int value);
int PWMWriteDutyCycle(int pwm_num, int value);
int PWMWriteDutyCycleByRatio(int pwm_num, float ratio);

#endif