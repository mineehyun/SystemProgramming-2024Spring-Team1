#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "pwm.c"
#include "lcd.c"
#include "gas.c"
#include "buzzer.c"
#include "rgb_led.c"  // Include the header for the RGB LED functions

#include <sys/socket.h>
#include <netinet/in.h>

int fd;
int gas_level;
pthread_mutex_t lock;

#define TEMPO 150  // beats per minute
#define EIGHT (60000 / (TEMPO * 2))
#define dotQUART (3 * EIGHT)  // duration of eighth note in ms
#define QUART (2 * EIGHT)  // Quarter note duration in ms
#define HALF (2 * QUART)
#define PORT 8080 // 바꺼야ㄷㅁㅇ ㅗㅗㅗㅗ

int fd;
int gas_level;
pthread_mutex_t lock;

int setupSocket() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

int waitForStartSignal(int socket) {
    char buffer[1] = {0};
    read(socket, buffer, 1);
    return buffer[0] == 0x01;
}



int main(void) {
    
    /* 소켓 통신 부분 */
    int server_socket = setupSocket();
    if (!waitForStartSignal(server_socket)) {
        printf("Did not receive start signal. Exiting.\n");
        close(server_socket);
        return 0;
    }

    printf("Received start signal. Starting threads.\n");
    close(server_socket);  // Close the server socket as it's no longer needed



    pthread_t thread_id_gas, thread_id_lcd, thread_id_buzzer, thread_id_rgb_led;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&thread_id_buzzer, NULL, buzzer_function, NULL);
    pthread_create(&thread_id_gas, NULL, gas_function, NULL);
    pthread_create(&thread_id_lcd, NULL, lcd_function, NULL);
    pthread_create(&thread_id_rgb_led, NULL, rgb_led_function, NULL);
    pthread_join(thread_id_buzzer, NULL);
    pthread_join(thread_id_gas, NULL);
    pthread_join(thread_id_lcd, NULL);
    pthread_join(thread_id_rgb_led, NULL);

    return 0;
}