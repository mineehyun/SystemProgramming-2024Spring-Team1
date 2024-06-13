#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pwm.c"
#include "../socket.c"
#include "../buzzer.c"
#include "../socket.c"
#include "gas.c"
#include "rgb_led.c"

int gas_level;
pthread_mutex_t lock;

#define TEMPO 150 // beats per minute
#define EIGHT (60000 / (TEMPO * 2))
#define dotQUART (3 * EIGHT) // duration of eighth note in ms
#define QUART (2 * EIGHT)    // Quarter note duration in ms
#define HALF (2 * QUART)

int gas_level;
pthread_mutex_t lock;
volatile int shut = 0;

int main(void)
{
    // int sock = socket_client("192.168.14.10", 8884);
    uint8_t buffer;
    int __result;
    while (1)
    {
        pthread_t thread_id_gas, thread_id_lcd, thread_id_buzzer, thread_id_rgb_led;
        pthread_mutex_init(&lock, NULL);

        pthread_create(&thread_id_gas, NULL, gas_function, NULL);
        // pthread_create(&thread_id_lcd, NULL, lcd_function, NULL);
        pthread_create(&thread_id_rgb_led, NULL, rgb_led_function, NULL);
        pthread_join(thread_id_gas, NULL);
        // pthread_join(thread_id_lcd, NULL);
        pthread_join(thread_id_rgb_led, NULL);
    }
    return 0;
}
