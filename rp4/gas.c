#include <wiringPi.h>
#include <mcp3004.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pwm.h"
#include "buzzer.h"
#include "gas.h"

extern int gas_level;  // Shared variable for the gas level
extern pthread_mutex_t lock;  // Mutex for synchronization

#define BASE 100
#define SPI_CHAN 0

void *gas_function(void *vargp) {
    int local_gas_value;
    int threshold = 200;  // Threshold for gas detection

    if (wiringPiSetup() == -1) exit(1);
    mcp3004Setup(BASE, SPI_CHAN);

    while (1) {
        local_gas_value = analogRead(BASE);
        pthread_mutex_lock(&lock);
        gas_level = local_gas_value;  // Update the shared variable within a lock
        printf("Current gas value: %d\n", gas_level);

        pthread_mutex_unlock(&lock);
        delay(3000);  // Check every 5 seconds
    }
    return NULL;
}
