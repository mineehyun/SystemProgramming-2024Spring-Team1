#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

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

typedef struct {
    int addr;
    int bus;
    int file;
} I2CDevice;

int i2c_open(I2CDevice *dev, int addr, int bus) {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", bus);
    dev->file = open(filename, O_RDWR);
    if (dev->file < 0) {
        perror("Failed to open the i2c bus");
        return -1;
    }
    dev->addr = addr;
    dev->bus = bus;
    if (ioctl(dev->file, I2C_SLAVE, dev->addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return -1;
    }
    return 0;
}

void i2c_close(I2CDevice *dev) {
    close(dev->file);
}

void i2c_write_cmd(I2CDevice *dev, uint8_t cmd) {
    if (write(dev->file, &cmd, 1) != 1) {
        perror("Failed to write command to the i2c bus");
    }
    usleep(100);
}

void i2c_write_cmd_arg(I2CDevice *dev, uint8_t cmd, uint8_t data) {
    uint8_t buffer[2] = {cmd, data};
    if (write(dev->file, buffer, 2) != 2) {
        perror("Failed to write command and argument to the i2c bus");
    }
    usleep(100);
}

void i2c_write_block_data(I2CDevice *dev, uint8_t cmd, uint8_t *data, size_t length) {
    uint8_t buffer[length + 1];
    buffer[0] = cmd;
    memcpy(buffer + 1, data, length);
    if (write(dev->file, buffer, length + 1) != length + 1) {
        perror("Failed to write block data to the i2c bus");
    }
    usleep(100);
}

uint8_t i2c_read(I2CDevice *dev) {
    uint8_t data;
    if (read(dev->file, &data, 1) != 1) {
        perror("Failed to read data from the i2c bus");
    }
    return data;
}

uint8_t i2c_read_data(I2CDevice *dev, uint8_t cmd) {
    if (write(dev->file, &cmd, 1) != 1) {
        perror("Failed to write read command to the i2c bus");
    }
    uint8_t data;
    if (read(dev->file, &data, 1) != 1) {
        perror("Failed to read data from the i2c bus");
    }
    return data;
}

void i2c_read_block_data(I2CDevice *dev, uint8_t cmd, uint8_t *buffer, size_t length) {
    if (write(dev->file, &cmd, 1) != 1) {
        perror("Failed to write read command to the i2c bus");
    }
    if (read(dev->file, buffer, length) != length) {
        perror("Failed to read block data from the i2c bus");
    }
}

void lcd_strobe(I2CDevice *lcd, uint8_t data) {
    i2c_write_cmd(lcd, data | En | LCD_BACKLIGHT);
    usleep(500);
    i2c_write_cmd(lcd, ((data & ~En) | LCD_BACKLIGHT));
    usleep(100);
}

void lcd_write_four_bits(I2CDevice *lcd, uint8_t data) {
    i2c_write_cmd(lcd, data | LCD_BACKLIGHT);
    lcd_strobe(lcd, data);
}

void lcd_write(I2CDevice *lcd, uint8_t cmd, uint8_t mode) {
    lcd_write_four_bits(lcd, mode | (cmd & 0xF0));
    lcd_write_four_bits(lcd, mode | ((cmd << 4) & 0xF0));
}

void lcd_display_string(I2CDevice *lcd, const char *string, int line) {
    switch (line) {
        case 1: lcd_write(lcd, 0x80, 0); break;
        case 2: lcd_write(lcd, 0xC0, 0); break;
        case 3: lcd_write(lcd, 0x94, 0); break;
        case 4: lcd_write(lcd, 0xD4, 0); break;
    }
    while (*string) {
        lcd_write(lcd, *string++, Rs);
    }
}

void lcd_clear(I2CDevice *lcd) {
    lcd_write(lcd, LCD_CLEARDISPLAY, 0);
    lcd_write(lcd, LCD_RETURNHOME, 0);
}

void lcd_backlight(I2CDevice *lcd, int state) {
    i2c_write_cmd(lcd, state ? LCD_BACKLIGHT : LCD_NOBACKLIGHT);
}

int main() {
    I2CDevice lcd;
    if (i2c_open(&lcd, 0x27, BUS_NUMBER) < 0) {
        return -1;
    }

    lcd_write(&lcd, 0x03, 0);
    lcd_write(&lcd, 0x03, 0);
    lcd_write(&lcd, 0x03, 0);
    lcd_write(&lcd, 0x02, 0);

    lcd_write(&lcd, LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE, 0);
    lcd_write(&lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON, 0);
    lcd_write(&lcd, LCD_CLEARDISPLAY, 0);
    lcd_write(&lcd, LCD_ENTRYMODESET | LCD_ENTRYLEFT, 0);

    usleep(200000);

    while(1){
    lcd_backlight(&lcd,0);
    lcd_display_string(&lcd, "                ", 1);
    lcd_display_string(&lcd, "                ", 2);
    lcd_display_string(&lcd, "JaeYukBoeckUmm", 1);
    lcd_display_string(&lcd, "12345", 2);
    usleep(5000000);
    
    lcd_backlight(&lcd,1);
    lcd_display_string(&lcd, "                ", 1);
    lcd_display_string(&lcd, "GoGiho.....", 1);
    lcd_display_string(&lcd, "                ", 2);
    lcd_display_string(&lcd, "BBang!", 2);
    usleep(5000000);
    }
    i2c_close(&lcd);
    
    return 0;
}
