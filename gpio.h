#ifndef __GPIO_H_
#define __GPIO_H_

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio%d/value"

#ifndef BUFLEN
#define BUFLEN 256
#endif

typedef enum
{
    IN = 0,
    OUT = 1,
} gpio_direction;

typedef enum
{
    LOW = 0,
    HIGH = 1,
} gpio_value;

/**
 * @returns
 * 0 if success, -1 if error.
 */
int gpio_export(int pin);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int gpio_unexport(int pin);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int gpio_set_direction(int pin, gpio_direction direction);

/**
 * @returns
 * 0 if success, -1 if error.
 */
int gpio_write(int pin, gpio_value value);

/**
 * @param value
 * 이 포인터에 읽은 값 저장됨요. 리턴값은 오류검출용.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int gpio_read(int pin, gpio_value *value);

#endif