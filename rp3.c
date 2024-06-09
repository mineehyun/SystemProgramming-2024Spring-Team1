#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "gpio.c"
#include "pwm.c"
#include "socket.c"
#include "buzzer.c"
#include "ultrasonic.c"

#define TEMPO 150  // beats per minute
#define EIGHT (60000 / (TEMPO * 2))
#define dotQUART (3 * EIGHT)  // duration of eighth note in ms
#define QUART (2 * EIGHT)  // Quarter note duration in ms
#define HALF (2 * QUART)

int main()
{
    int melody[] = {
        D5, G5, F5, E5, D5, B4,
        C5,D5,E5,A4,B4,C5, B4, D5,

        D5,Gs5,F5,E5,D5,G5,
        G5,A5,G5,F5,E5,F5,G5
    };
    int durations[] = {
       dotQUART, EIGHT , EIGHT, EIGHT, dotQUART,dotQUART,
       EIGHT,EIGHT,EIGHT,EIGHT,EIGHT,EIGHT,dotQUART,dotQUART,
       dotQUART,EIGHT,EIGHT,EIGHT,dotQUART,dotQUART,
       EIGHT,EIGHT,EIGHT,EIGHT,EIGHT,EIGHT,HALF
    };

    unsigned int score_length = 27;
    note *score = (note *)malloc(sizeof(note) * score_length);
    for (int i = 0; i < score_length; i++)
    {
        score[i].pitch = melody[i];
        score[i].duration = (double)durations[i] / 1000;
    }
    buzzer_thread_args *args = (buzzer_thread_args *)malloc(sizeof(buzzer_thread_args));
    args->pwm_num = 0;
    args->score = score;
    args->score_length = score_length;
    pthread_create(&args->tid, NULL, buzzer_thread, args);
    pthread_join(args->tid, NULL);
    return 0;
}   