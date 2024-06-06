#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

#define SPI_PATH "/dev/spidev0.0"
#define SPI_SPEED 1000000
#define SPI_MODE SPI_MODE_0
#define SPI_BITS_PER_WORD 8

/**
 * @brief
 * Open SPI device and initialize it.
 * Device must be closed.
 * 
 * @return
 * SPI file descriptor. -1 if error.
 */
int spi_init();

/**
 * @brief
 * Send a byte of data to SPI device,
 * and receive a byte of data from SPI device.
 * 
 * @param spi_fd
 * SPI device file descriptor.
 * @param tx
 * A byte of data to be sent to SPI device.
 * 
 * @return
 * A byte of data received. 0 if error.
 */
uint8_t spi_transfer(int spi_fd, uint8_t tx);

#endif