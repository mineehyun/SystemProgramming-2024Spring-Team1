#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <pthread.h>
#include "lcd.h"

#define I2C_ADDR 0x27
#define LCD_CHR 1
#define LCD_CMD 0
#define LINE1 0x80
#define LINE2 0xC0
#define LCD_BACKLIGHT 0x08
#define ENABLE 0b00000100

extern int fd;  // File descriptor for the I2C connection
extern int gas_level;  // Shared variable for the gas level
extern pthread_mutex_t lock;  // Mutex for synchronization

// Function prototypes
void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);
void lcdLoc(int line);
void ClrLcd(void);
void typeln(const char *s);
void *lcd_function(void *vargp);

void lcd_init() {
    lcd_byte(0x33, LCD_CMD);  // Function set
    lcd_byte(0x32, LCD_CMD);  // Function set
    lcd_byte(0x06, LCD_CMD);  // Entry mode set
    lcd_byte(0x0C, LCD_CMD);  // Display on, cursor off, blink off
    lcd_byte(0x28, LCD_CMD);  // Data length, number of lines, font size
    lcd_byte(0x01, LCD_CMD);  // Clear display
    delayMicroseconds(500);
}

void lcd_byte(int bits, int mode) {
    int bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    int bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    wiringPiI2CReadReg8(fd, bits_high);
    lcd_toggle_enable(bits_high);
    wiringPiI2CReadReg8(fd, bits_low);
    lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits) {
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits | ENABLE));
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
    delayMicroseconds(500);
}

void lcdLoc(int line) {
    lcd_byte(line, LCD_CMD);
}

void ClrLcd() {
    lcd_byte(0x01, LCD_CMD);  // Clear display
    lcd_byte(0x02, LCD_CMD);  // Home cursor
}

void typeln(const char *s) {
    while (*s) {
        lcd_byte(*(s++), LCD_CHR);
    }
}

void *lcd_function(void *vargp) {
    fd = wiringPiI2CSetup(I2C_ADDR);
    lcd_init();
    while (1) {
        int local_gas_level;

        pthread_mutex_lock(&lock);
        local_gas_level = gas_level;
        if (local_gas_level > 230) {
            lcdLoc(LINE1);
            typeln("Gas! Gas! Gas!");
            lcdLoc(LINE2);
            typeln("Gas! Gas! Gas!");
        } else {
            lcdLoc(LINE1);
            typeln("Welcome!!!!");
            lcdLoc(LINE2);
            typeln("You are safe now");
        }
        pthread_mutex_unlock(&lock);
        delay(3000);
        ClrLcd();
    }
    return NULL;
}
