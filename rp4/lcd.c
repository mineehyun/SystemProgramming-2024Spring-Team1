#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "../gpio.h"
#include "lcd.h"

extern int gas_level;
extern int gas_threshold;
extern pthread_mutex_t lock;

int lcd_init(lcd *lcd, int addr, int bus)
{
    char buffer[20];
    snprintf(buffer, 19, "/dev/i2c-%d", bus);
    lcd->fd = open(buffer, O_RDWR);
    if (lcd->fd < 0)
    {
        perror("[lcd_init] open");
        return -1;
    }
    lcd->addr = addr;
    lcd->bus = bus;
    if (ioctl(lcd->fd, I2C_SLAVE, lcd->addr) < 0)
    {
        perror("[lcd_init] ioctl");
        return -1;
    }
    // Init lcd by 채찍피티
    lcd_write_byte(lcd, 0x03, 0);
    lcd_write_byte(lcd, 0x03, 0);
    lcd_write_byte(lcd, 0x03, 0);
    lcd_write_byte(lcd, 0x02, 0);
    lcd_write_byte(lcd, LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE, 0);
    lcd_write_byte(lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON, 0);
    lcd_write_byte(lcd, LCD_CLEARDISPLAY, 0);
    lcd_write_byte(lcd, LCD_ENTRYMODESET | LCD_ENTRYLEFT, 0);
    usleep(200000);
    return 0;
}

void i2c_write(lcd *lcd, uint8_t byte)
{
    if (write(lcd->fd, &byte, 1) == -1)
    {
        perror("[i2c_write] write");
    }
    usleep(100);
}

void lcd_strobe(lcd *lcd, uint8_t byte)
{
    i2c_write(lcd, byte | En | LCD_BACKLIGHT);
    usleep(500);
    i2c_write(lcd, ((byte & ~En) | LCD_BACKLIGHT));
    usleep(100);
}

void lcd_write_half_byte(lcd *lcd, uint8_t byte)
{
    i2c_write(lcd, byte | LCD_BACKLIGHT);
    lcd_strobe(lcd, byte);
}

void lcd_write_byte(lcd *lcd, uint8_t byte, uint8_t mode)
{
    lcd_write_half_byte(lcd, mode | (byte & 0xF0));
    lcd_write_half_byte(lcd, mode | ((byte << 4) & 0xF0));
}

int lcd_display(lcd *lcd, char string[], int line)
{
    if (line == LINE1)
    {
        lcd_write_byte(lcd, 0x80, 0);
    }
    else if (line == LINE2)
    {
        lcd_write_byte(lcd, 0xC0, 0);
    }
    else
    {
        return -1;
    }
    while (*string)
    {
        lcd_write_byte(lcd, *string++, Rs);
    }
    return 0;
}

void lcd_clear(lcd *lcd)
{
    lcd_write_byte(lcd, LCD_CLEARDISPLAY, 0);
    lcd_write_byte(lcd, LCD_RETURNHOME, 0);
}

void *lcd_function(void *vargp)
{
    lcd lcd;
    int fd = lcd_init(&lcd, 0x27, BUS_NUMBER);
    if (fd == -1)
    {
        perror("[lcd_function] lcd_init");
        return NULL;
    }
    int local_gas_level, gaswas = 1;
    while (1)
    {
        pthread_mutex_lock(&lock);
        local_gas_level = gas_level;
        pthread_mutex_unlock(&lock);
        if (local_gas_level > gas_threshold)
        {
            // 상태변화만 감지
            if (gaswas == 0)
            {
                lcd_clear(&lcd);
                lcd_display(&lcd, "Gas! Gas! Gas!", LINE1);
                lcd_display(&lcd, "Gas! Gas! Gas!", LINE2);
                gpio_export(23);
                gpio_set_direction(23, OUT);
                gpio_write(23, HIGH);
                gaswas = 1;
            }
        }
        else
            // 상태변화만 감지
            if (gaswas == 1)
            {
                {
                    lcd_clear(&lcd);
                    lcd_display(&lcd, "Welcome!!!!", LINE1);
                    lcd_display(&lcd, "You are safe now.", LINE2);
                    gpio_write(23, LOW);
                    gpio_unexport(23);
                    gaswas = 0;
                }
            }
        usleep(100000);
    }
    return NULL;
}
