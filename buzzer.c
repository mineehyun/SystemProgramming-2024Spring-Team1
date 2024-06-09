#include <stdio.h>
#include <stdint.h>
#include "buzzer.h"

int __buzzer_play_note(pwm_num pwm_num, note *note)
{
    uint32_t period = 1000000000 / note->pitch;
    uint32_t duty_cycle = period / 2;
    printf("period %d dur %2lf\n", period, note->duration);
    if (pwm_write_period(pwm_num, period) == -1)
    {
        perror("[__buzzer_play_note] pwm_write_period");
        return -1;
    }
    if (pwm_write_duty_cycle(pwm_num, duty_cycle) == -1)
    {
        perror("[__buzzer_play_note] pwm_write_duty_cycle");
        return -1;
    }
    usleep((int)(note->duration * 1000000));
    return 0;
}

void __buzzer_thread_finalize(void *args)
{
    buzzer_thread_args *__args = (buzzer_thread_args *)args;
    pwm_disable(__args->pwm_num);
    pwm_unexport(__args->pwm_num);
    free(__args->score);
    free(__args);
}

void *buzzer_thread(void *args)
{
    buzzer_thread_args *__args = (buzzer_thread_args *)args;
    pwm_export(__args->pwm_num);
    pwm_enable(__args->pwm_num);
    usleep(1000);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(__buzzer_thread_finalize, args);
    for (note *__note = __args->score; __note < __args->score + __args->score_length; __note++)
    {
        __buzzer_play_note(__args->pwm_num, __note);
    }
    pthread_cleanup_pop(1);
    return NULL;
}