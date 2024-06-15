#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"

int spi_init()
{
    int spi_fd = open(SPI_PATH, O_RDWR);
    if (spi_fd == -1)
    {
        perror("[spi_init] open");
        return -1;
    }
    /* Init SPI device */
    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS_PER_WORD;
    uint32_t speed = SPI_SPEED;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1 ||
        ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1 ||
        ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
    {
        perror("[spi_init] ioctl");
        close(spi_fd);
        return -1;
    }
    return spi_fd;
}

int spi_transfer(int spi_fd, uint8_t tx, uint8_t *rx)
{
    struct spi_ioc_transfer tr = {0};
    tr.tx_buf = (unsigned long)&tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 1;
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) == -1)
    {
        perror("[spi_transfer] ioctl");
        return -1;
    }
    return 0;
}