#ifndef __PWM_H__
#define __PWM_H__

#define PWM_EXPORT_PATH "/sys/class/pwm/pwmchip0/export"
#define PWM_UNEXPORT_PATH "/sys/class/pwm/pwmchip0/unexport"
#define PWM_ENABLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/enable"
#define PWM_PERIOD_PATH "/sys/class/pwm/pwmchip0/pwm%d/period"
#define PWM_DUTYCYCLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle"

#define DEFAULT_PERIOD 1000000

#ifndef BUFLEN
#define BUFLEN 256
#endif

void PWMExport(int pwm_num);
void PWMUnexport(int pwm_num);
void PWMEnable(int pwm_num);
void PWMDisable(int pwm_num);
void PWMWritePeriod(int pwm_num, int value);
void PWMWriteDutyCycle(int pwm_num, int value);
void PWMWriteRatio(int pwm_num, float ratio);

#endif