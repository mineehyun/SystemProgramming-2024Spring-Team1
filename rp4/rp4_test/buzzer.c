#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <pthread.h>
#include "buzzer.h"

#define BUZZER_PIN 1  // wiringPi pin number 1 is GPIO 18

// Note frequencies (in hertz)
#define B3 246.94
#define A3 220 
#define C4  261.63
#define Cs4 277.18  // C#4 or Db4
#define D4  293.66
#define Ds4 311.13  // D#4 or Eb4
#define E4  329.63
#define F4  349.23
#define Fs4 369.99  // F#4 or Gb4
#define G4  392.00
#define Gs4 415.30  // G#4 or Ab4
#define A4  440.00
#define As4 466.16  // A#4 or Bb4
#define B4  493.88
#define C5  523.25
#define Cs5 554.37  // C#5 or Db5
#define D5  587.33
#define Ds5 622.25  // D#5 or Eb5
#define E5  659.26
#define F5  698.46
#define Fs5 739.99  // F#5 or Gb5
#define G5  783.99
#define Gs5 830.61  // G#5 or Ab5
#define A5  880.00
#define As5 932.33  // A#5 or Bb5
#define B5  987.77


#define SIREN_MIN 440  // Starting frequency for the siren
#define SIREN_MAX 880

// Note durations (in milliseconds)
#define TEMPO 150  // beats per minute
#define EIGHT (60000 / (TEMPO * 2))
#define dotQUART (3 * EIGHT)  // duration of eighth note in ms
#define QUART (2 * EIGHT)  // Quarter note duration in ms
#define HALF (2 * QUART)

extern int gas_level;  // Shared variable for the gas level
extern pthread_mutex_t lock;  // Mutex for synchronization
volatile int shut;

void setupBuzzer() {
    //wiringPiSetup();                // Initialize wiringPi library
    softToneCreate(BUZZER_PIN);     // Set the buzzer pin to softTone mode
}

void playAirRaidSiren(int pin, int duration) {
    int stepDuration = 50;  
    int steps = duration / (2 * stepDuration);  // Total number of steps up and down

    for (int up = 1; up >= -1; up -= 2) {  // Combine up and down into one loop
        for (int freq = (up > 0 ? SIREN_MIN : SIREN_MAX); 
             (up > 0 ? freq <= SIREN_MAX : freq >= SIREN_MIN); 
             freq += (up * (SIREN_MAX - SIREN_MIN) / steps)) {

            pthread_mutex_lock(&lock);
            if (gas_level <= 300) {
                pthread_mutex_unlock(&lock);
                softToneWrite(pin, 0);  // Turn off the tone
                return;  // Stop playing siren if gas level goes back to normal
            }
            pthread_mutex_unlock(&lock);

            softToneWrite(pin, freq);
            delay(stepDuration);
        }
    }
    softToneWrite(pin, 0); 

}

void playLGJingle() {
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

    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
        pthread_mutex_lock(&lock);
        if (gas_level > 300) {
            pthread_mutex_unlock(&lock);
            return;  // Immediately stop playing the melody if gas level is high
        }
        pthread_mutex_unlock(&lock);

        softToneWrite(BUZZER_PIN, melody[i]);
        delay(durations[i]);
        softToneWrite(BUZZER_PIN, 0);  // Stop the tone between notes
        delay(EIGHT / 10);
    }
}


void *buzzer_function(void *vargp) {
    int welcomeflag = 1;
    int gasflag = 2;
    setupBuzzer();  // Setup buzzer

    while (1) {
        pthread_mutex_lock(&lock);
        if (gas_level > 300 && gasflag) {
            pthread_mutex_unlock(&lock);
            gasflag--;
            playAirRaidSiren(BUZZER_PIN, 2000);  // Play siren for 2 seconds if gas level is high
        } else if(welcomeflag) {
            pthread_mutex_unlock(&lock);
            welcomeflag--;
            playLGJingle();  // Play melody otherwise
        } else if (!welcomeflag && !gasflag) {
            break;
        }
        delay(100);  // Short delay to prevent CPU overload
    }

    return NULL;
}
