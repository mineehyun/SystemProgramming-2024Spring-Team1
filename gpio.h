#ifndef __GPIO_H_
#define __GPIO_H_

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio%d/value"

typedef enum __gpio_direction
{
    IN = 0,
    OUT = 1,
} gpio_direction;

typedef enum __gpio_value
{
    LOW = 0,
    HIGH = 1,
} gpio_value;

#ifndef BUFLEN
#define BUFLEN 256
#endif

void GPIOExport(int pin);
void GPIOUnexport(int pin);
void GPIODirection(int pin, gpio_direction direction);
void GPIOWrite(int pin, gpio_value status);
gpio_value GPIORead(int pin);

#endif