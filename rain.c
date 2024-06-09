
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "gpio1.h"

#define NO_RAIN_GPIO 14
// flag == 1: no_rain 
// flag == 0: yes_rain

void* rain_function(void* rain) {
    GPIOExport(NO_RAIN_GPIO);
    GPIODirection(NO_RAIN_GPIO, 0);
    int rain_flag = GPIORead(NO_RAIN_GPIO);
    rain = &rain_flag;
    printf("%d", rain_flag);

    GPIOUnexport(NO_RAIN_GPIO);
    return rain;
}
