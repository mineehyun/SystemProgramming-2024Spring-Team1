
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "gpio1.h"

#define NO_RAIN_GPIO 14

int main() {
    GPIOExport(NO_RAIN_GPIO);
    GPIODirection(NO_RAIN_GPIO, 0);

    while (1) {
        int no_rain = GPIORead(NO_RAIN_GPIO);
        
        if (no_rain == 1) {
            printf("nothingssssss\n");
        } else if (no_rain == 0) {
            printf("It's raining - get the washing in!\n");
        }
        sleep(1);
    }
    GPIOUnexport(NO_RAIN_GPIO);
    return 0;
}
