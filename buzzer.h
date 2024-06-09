#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <pthread.h>
#include "pwm.h"

typedef enum
{
    B3 = 247,
    A3 = 220,
    C4 = 262,
    Cs4 = 277,
    D4 = 294,
    Ds4 = 311,
    E4 = 330,
    F4 = 349,
    Fs4 = 367,
    G4 = 392,
    Gs4 = 415,
    A4 = 440,
    As4 = 466,
    B4 = 494,
    C5 = 523,
    Cs5 = 554,
    D5 = 587,
    Ds5 = 622,
    E5 = 659,
    F5 = 698,
    Fs5 = 740,
    G5 = 784,
    Gs5 = 831,
    A5 = 880,
    As5 = 932,
    B5 = 988,
} pitch;

typedef struct
{
    pitch pitch;
    double duration;
} note;

typedef struct
{
    pthread_t tid;
    pwm_num pwm_num;
    note *score;
    unsigned int score_length;
} buzzer_thread_args;

typedef struct
{
    pthread_t tid;
    pwm_num pwm_num;
    uint32_t freq_min;
    uint32_t freq_max;
} siren_thread_args;

/**
 * Plays a single note. Do not manipulates pwm/gpio pins.
 *
 * @param pwm_num
 * 생각.
 * @param note
 * 뭐겠니?
 * 
 * @returns
 * 0 if success, -1 if error.
 */
int __buzzer_play_note(pwm_num pwm_num, note *note);

/**
 * Thread finalize function of `buzzer_thread`.
 * 
 * @param args
 * Use type `buzzer_thread_args *`
 */
void __buzzer_thread_finalize(void *args);

/**
 * Plays a score. Also set PWM pins.
 * 
 * @param args
 * Use type `buzzer_thread_args *`
 */
void *buzzer_thread(void *args);

/**
 * Thread finalize function of `siren_thread`.
 * 
 * @param args
 * Use type `siren_thread_args *`
 */
void __siren_thread_finalize(void *args);

/**
 * Siren thread. Also set PWM pins.
 * 
 * @param args
 * Use type `siren_thread_args *`
 */
void *siren_thread(void *args);

#endif