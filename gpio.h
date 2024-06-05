#ifndef __GPIO_H_
#define __GPIO_H_

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio%d/value"
#define IN 0
#define OUT 1
#define LOW 0
#define HIGH 1

#ifndef BUFLEN
#define BUFLEN 256
#endif

void GPIOExport(int pin);
void GPIOUnexport(int pin);
void GPIODirection(int pin, int dir);
void GPIOWrite(int pin, int value);
int GPIORead(int pin);

#endif