#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"

int spi_init() {
    int spi_fd;
    if ((spi_fd = open(SPI_PATH, O_RDWR)) < 0) {
        perror("[SPI init] open failed");
        return -1;
    }
    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS_PER_WORD;
    uint32_t speed = SPI_SPEED;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) {
        perror("[SPI init] mode set failed");
        close(spi_fd);
        return -1;
    }
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        perror("[SPI init] bits per word set failed");
        close(spi_fd);
        return -1;
    }
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        perror("[SPI init] speed set failed");
        close(spi_fd);
        return -1;
    }
    return spi_fd;
}

uint8_t spi_transfer(int spi_fd, uint8_t tx) {
    uint8_t rx;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)&tx,
        .rx_buf = (unsigned long)&rx,
        .len = 1,
    };
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        perror("[SPI transfer] transfer failed");
        return 0;
    }
    return rx;
}