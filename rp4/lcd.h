#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <fcntl.h>

// I2C 버스 번호 설정
#define BUS_NUMBER 1

// LCD 명령 상수
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// 표시기 모드 플래그
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// 디스플레이 켜기/끄기 제어 플래그
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// 디스플레이/커서 이동 플래그
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// 기능 설정 플래그
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// 백라이트 제어 플래그
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// GPIO 비트 설정
#define En 0b00000100
#define Rw 0b00000010
#define Rs 0b00000001

// 라인 설정
#define LINE1 1
#define LINE2 2

typedef struct {
    int addr;
    int bus;
    int fd;
} lcd;

int lcd_init(lcd *lcd, int addr, int bus);

void i2c_write(lcd *lcd, uint8_t byte);

void lcd_strobe(lcd *lcd, uint8_t byte);

void lcd_write_half_byte(lcd *lcd, uint8_t byte);

void lcd_write_byte(lcd *lcd, uint8_t byte, uint8_t mode);

int lcd_display(lcd *lcd, char string[], int line);

void lcd_clear(lcd *lcd);

#endif