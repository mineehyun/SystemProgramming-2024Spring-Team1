#ifndef __GPIO_H_
#define __GPIO_H_

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio%d/value"
#define GPIO_IN 0
#define GPIO_OUT 1

#ifndef BUFLEN
#define BUFLEN 256
#endif

int GPIOExport(int pin_num);
int GPIOUnexport(int pin_num);
int GPIODirection(int pin_num, int direction);
int GPIORead(int pin_num);
int GPIOWrite(int pin_num, int value);

#endif