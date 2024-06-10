#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "pwm.c"
#include "lcd.c"
#include "gas.c"
#include "buzzer.c"
#include "rgb_led.c"
#include "socket.h"  // Ensure this header is properly set up to expose socket_server and socket_client

int gas_level;
pthread_mutex_t lock;

#define PORT 8080  // Define the port on which the server will listen

int main(void) {
    pthread_t thread_id_gas, thread_id_lcd, thread_id_buzzer, thread_id_rgb_led;

    // Initialize server socket to listen for start commands
    int client_sock = socket_server(htons(PORT));
    if (client_sock == -1) {
        fprintf(stderr, "Failed to setup socket server\n");
        return 1;
    }

    // Receive the start signal
    char start_signal;
    ssize_t recv_size = read(client_sock, &start_signal, sizeof(start_signal));
    if (recv_size > 0 && start_signal == 0x01) {
        printf("Received start signal. Initializing threads.\n");
        
        pthread_mutex_init(&lock, NULL);

        pthread_create(&thread_id_buzzer, NULL, buzzer_function, NULL);
        pthread_create(&thread_id_gas, NULL, gas_function, NULL);
        pthread_create(&thread_id_lcd, NULL, lcd_function, NULL);
        pthread_create(&thread_id_rgb_led, NULL, rgb_led_function, NULL);

        pthread_join(thread_id_buzzer, NULL);
        pthread_join(thread_id_gas, NULL);
        pthread_join(thread_id_lcd, NULL);
        pthread_join(thread_id_rgb_led, NULL);
    } else {
        printf("No valid start signal received or error in receiving data.\n");
    }

    close(client_sock);  // Close the client socket after use
    return 0;
}
