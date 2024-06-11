#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "gpio1.c"

#define NO_RAIN_GPIO 17
// flag == 1: no_rain 
// flag == 0: yes_rain

void* rain_function(void* rain) {
    int* rainF = (int*)rain;
    GPIOExport(NO_RAIN_GPIO);
    GPIODirection(NO_RAIN_GPIO, 0);
    *rainF = !GPIORead(NO_RAIN_GPIO);
    GPIOUnexport(NO_RAIN_GPIO);
    return rainF;
}
