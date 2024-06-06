#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#define BCM2708_PERI_BASE        0x3F000000  // Raspberry Pi 2/3
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
#define INP_GPIO(g) *(gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define GPIO_READ(g) (*(gpio + 13) & (1 << g)) // GPLEV0 레지스터에서 읽기

volatile unsigned *gpio;

int setup_io() {
    int mem_fd;
    void *gpio_map;

    // /dev/mem 열기
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        perror("can't open /dev/mem");
        return -1;
    }

    // GPIO 메모리 매핑
    gpio_map = mmap(
        NULL,                 // Any address in our space will do
        BLOCK_SIZE,           // Map length
        PROT_READ|PROT_WRITE, // Enable reading & writing to mapped memory
        MAP_SHARED,           // Shared with other processes
        mem_fd,               // File to map
        GPIO_BASE             // Offset to GPIO peripheral
    );

    close(mem_fd); // 메모리 맵핑 후 파일 디스크립터 닫기

    if (gpio_map == MAP_FAILED) {
        perror("mmap error");
        return -1;
    }

    // 포인터 설정
    gpio = (volatile unsigned *)gpio_map;

    return 0;
}

int readBit(int pin) {
    if (pin < 0 || pin > 53) {
        fprintf(stderr, "Invalid GPIO pin number\n");
        return -1;
    }

    INP_GPIO(pin); // 핀을 입력 모드로 설정

    if (GPIO_READ(pin)) {
        return 1; // HIGH
    } else {
        return 0; // LOW
    }
}

unsigned char readByte(int startPin) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) {
        int pin = startPin + i;
        int bit = readBit(pin);
        if (bit < 0) {
            fprintf(stderr, "Failed to read GPIO pin %d\n", pin);
            return 0xFF; // 오류 시 0xFF 반환
        }
        result |= (bit << i);
    }
    return result;
}

void read40Bits(int pin, unsigned char *buffer) {
    for (int i = 0; i < 40; i++) {
        int bit = readBit(pin);
        if (bit < 0) {
            fprintf(stderr, "Failed to read GPIO pin %d\n", pin);
            buffer[0] = 0xFF; // 오류 시 첫 바이트를 0xFF로 설정하여 오류 표시
            return;
        }
        int byteIndex = i / 8;
        int bitIndex = i % 8;
        buffer[byteIndex] |= (bit << (7 - bitIndex)); // 비트를 역순으로 저장
    }
}

int main() {
    if (setup_io() < 0) {
        return 1;
    }

    int pin = 16; // GPIO 16번 핀 사용
    unsigned char buffer[5] = {0};

    read40Bits(pin, buffer);

    if (buffer[0] == 0xFF) {
        fprintf(stderr, "Failed to read GPIO pins\n");
    } else {
        printf("Read 40 bits: ");
        for (int i = 0; i < 5; i++) {
            printf("0x%02X ", buffer[i]);
        }
        printf("\n");
    }

    return 0;
}
