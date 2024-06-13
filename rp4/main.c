#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "pwm.c"
#include "lcd.c"
#include "gas.c"
#include "rgb_led.c" // Include the header for the RGB LED functions
#include "../buzzer.c"
#include "socket.c"
#include <sys/socket.h>
#include <netinet/in.h>

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
    if (wiringPiSetup() == -1) exit(1);
    mcp3004Setup(BASE, SPI_CHAN);

    int sock = socket_client("192.168.14.10", 8884);
    uint8_t buffer;
    int __result;
    while (1)
    {
        __result = read(sock, &buffer, 1);
        /* EOF. wait for new data */
        if (__result == 0)
        {
            usleep(100000);
            //printf("dd\n");
            continue;
        }
        /* Error */
        else if (__result == -1)
        {
            perror("[__rp1_thread] read");
            return 0;
        }
        /* 뭐라도 받았으면 철!!!!!컹!!!!!!!!! */
        // 여기에 쳐 넣기

        pthread_t thread_id_gas, thread_id_lcd, thread_id_buzzer, thread_id_rgb_led;
        pthread_mutex_init(&lock, NULL);

        pthread_create(&thread_id_buzzer, NULL, buzzer_function, NULL);
        pthread_create(&thread_id_gas, NULL, gas_function, NULL);
        pthread_create(&thread_id_lcd, NULL, lcd_function, NULL);
        pthread_create(&thread_id_rgb_led, NULL, rgb_led_function, NULL);
        
    

        // /* 철컹 끗 신호 받기 뭐라도 받으면 반복문 탈출 */
        // __result = read(sock, &buffer, 1);
        // /* EOF. wait for new data */
        // if (__result == 0)
        // {
        //     usleep(100000);
        //     //printf("dd\n");
        //     continue;
        // }
        // /* Error */
        // else if (__result == -1)
        // {
        //     perror("[__rp1_thread] read");
        //     return 0;
        // }

        

        /* 철컹 끗 신호 받음 */
        pthread_join(thread_id_buzzer, NULL);
    }
    return 0;
}
