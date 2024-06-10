#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "pwm.c"
#include "lcd.c"
#include "gas.c"
#include "buzzer.c"
#include "rgb_led.c"
#include "socket.h"

int main(void) {
    pthread_t tid;
    pthread_create(&tid, )
    if (recv_size > 0 && start_signal == 0x01) {
        printf("Received start signal. Initializing threads.\n");
        
        pthread_mutex_init(&lock, NULL);

        pthread_create(&thread_id_buzzer, NULL, buzzer_function, NULL);

        pthread_join(thread_id_buzzer, NULL);
    } else {
        printf("No valid start signal received or error in receiving data.\n");
    }

    close(client_sock);  // Close the client socket after use
    return 0;
}
