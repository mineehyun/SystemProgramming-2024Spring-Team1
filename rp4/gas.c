#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pwm.h"
#include "buzzer.h"
#include "gas.h"

void *gas_function(void *vargp) {
    int local_gas_value;
    int threshold = 200;  // Threshold for gas detection

    if (wiringPiSetup() == -1) exit(1);
    mcp3004Setup(BASE, SPI_CHAN);

    while (1) {
        local_gas_value = analogRead(BASE);
        printf("Current gas value: %d\n", local_gas_value);
        pthread_mutex_lock(&lock);
        gas_level = local_gas_value;  // Update the shared variable within a lock
        pthread_mutex_unlock(&lock);
        sleep(5);
    }
    return NULL;
}
