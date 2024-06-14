#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../pwm.c"
#include "../socket.c"
#include "../buzzer.c"
#include "../gpio.c"
#include "gas.c"
#include "lcd.c"
#include "rgb_led.c"

int gas_level;
int gas_threshold;
pthread_mutex_t lock;

int main(int argc, char *argv[])
{
    // set threshold
    gas_threshold = atoi(argv[1]);

    // // wait for signal from rp3
    // int sock = socket_client("192.168.14.10", 8884);
    // uint8_t buffer;
    // int __result;
    // while (1)
    // {
    //     __result = read(sock, &buffer, 1);
    //     /* EOF. wait for new data */
    //     if (__result == 0)
    //     {
    //         usleep(100000);
    //         //printf("dd\n");
    //         continue;
    //     }
    //     /* Error */
    //     else if (__result == -1)
    //     {
    //         perror("[__rp1_thread] read");
    //         return 0;
    //     }
    // }

    pthread_t thread_id_gas, thread_id_lcd, thread_id_rgb_led;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&thread_id_gas, NULL, gas_function, NULL);
    pthread_create(&thread_id_lcd, NULL, lcd_function, NULL);
    pthread_create(&thread_id_rgb_led, NULL, rgb_led_function, NULL);
    pthread_join(thread_id_gas, NULL);
    pthread_join(thread_id_lcd, NULL);
    pthread_join(thread_id_rgb_led, NULL);
    return 0;
}
