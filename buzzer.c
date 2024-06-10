#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "buzzer.h"

static int melody[] = {
    D5, G5, F5, E5, D5, B4,
    C5, D5, E5, A4, B4, C5, B4, D5,
    D5, Gs5, F5, E5, D5, G5,
    G5, A5, G5, F5, E5, F5, G5};

static int durations[] = {
    dotQUART, EIGHT, EIGHT, EIGHT, dotQUART, dotQUART,
    EIGHT, EIGHT, EIGHT, EIGHT, EIGHT, EIGHT, dotQUART, dotQUART,
    dotQUART, EIGHT, EIGHT, EIGHT, dotQUART, dotQUART,
    EIGHT, EIGHT, EIGHT, EIGHT, EIGHT, EIGHT, HALF};

int __buzzer_play_note(pwm_num pwm_num, note *note)
{
    uint32_t period = 1000000000 / note->pitch;
    uint32_t duty_cycle = period / 2;
    if (pwm_write_period(pwm_num, period) == -1)
    {
        perror("[__buzzer_play_note] pwm_write_period");
        usleep((int)(note->duration * 1000000));
        return -1;
    }
    if (pwm_write_duty_cycle(pwm_num, duty_cycle) == -1)
    {
        perror("[__buzzer_play_note] pwm_write_duty_cycle");
        usleep((int)(note->duration * 1000000));
        return -1;
    }
    usleep((int)(note->duration * 1000000));
    return 0;
}

void __buzzer_thread_finalize(void *args)
{
    buzzer_thread_args *__args = (buzzer_thread_args *)args;
    pwm_unexport(__args->pwm_num);
    free(__args->score);
}

void *buzzer_thread(void *args)
{
    buzzer_thread_args *__args = (buzzer_thread_args *)args;
    pwm_export(__args->pwm_num);
    pthread_cleanup_push(__buzzer_thread_finalize, args);
    pwm_enable(__args->pwm_num);
    usleep(1000);
    for (note *__note = __args->score; __note < __args->score + __args->score_length; __note++)
    {
        __buzzer_play_note(__args->pwm_num, __note);
    }
    pwm_disable(__args->pwm_num);
    pthread_cleanup_pop(1);
}

void __siren_thread_finalize(void *args)
{
    siren_thread_args *__args = (siren_thread_args *)args;
    pwm_unexport(__args->pwm_num);
}

void *siren_thread(void *args)
{
    siren_thread_args *__args = (siren_thread_args *)args;
    pwm_export(__args->pwm_num);
    pthread_cleanup_push(__siren_thread_finalize, args);
    pwm_enable(__args->pwm_num);
    usleep(1000);
    for (uint32_t __freq = __args->freq_min; __freq < __args->freq_max; __freq += 10)
    {
        note __note =
            {
                .duration = 0.01,
                .pitch = __freq,
            };
        __buzzer_play_note(__args->pwm_num, &__note);
    }
    for (uint32_t __freq = __args->freq_max; __freq > __args->freq_min; __freq -= 10)
    {
        note __note =
            {
                .duration = 0.01,
                .pitch = __freq,
            };
        __buzzer_play_note(__args->pwm_num, &__note);
    }
    pwm_disable(__args->pwm_num);
    pthread_cleanup_pop(1);
}