#ifndef __PWM_H__
#define __PWM_H__

#include <stdint.h>

#define PWM_EXPORT_PATH "/sys/class/pwm/pwmchip0/export"
#define PWM_UNEXPORT_PATH "/sys/class/pwm/pwmchip0/unexport"
#define PWM_ENABLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/enable"
#define PWM_PERIOD_PATH "/sys/class/pwm/pwmchip0/pwm%d/period"
#define PWM_DUTY_CYCLE_PATH "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle"

#define DEFAULT_PERIOD 1000000

#ifndef BUFLEN
#define BUFLEN 256
#endif

typedef enum
{
    PWM0 = 0,
    PWM1 = 1,
} pwm_num;

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_export(pwm_num pwm_num);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_unexport(pwm_num pwm_num);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_enable(pwm_num pwm_num);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_disable(pwm_num pwm_num);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_write_period(pwm_num pwm_num, uint32_t period);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_write_duty_cycle(pwm_num pwm_num, uint32_t duty_cycle);

/**
 * Call `pwm_write_period` and `pwm_write_duty_cycle` at once.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int pwm_ratio(pwm_num pwm_num, float ratio);

#endif