#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

#define SPI_PATH "/dev/spidev0.0"
#define SPI_SPEED 500000
#define SPI_MODE SPI_MODE_0
#define SPI_BITS_PER_WORD 8

/**
 * Open SPI device and initialize it.
 * 
 * @returns
 * SPI fd. -1 if error.
 */
int spi_init();

/**
 * Through SPI protocol, send a byte of data `tx` and receive a byte of data via `*rx`.
 * 
 * @param spi_fd
 * SPI device file descriptor.
 * @param tx
 * 보낼거
 * @param rx
 * 받는곳
 * 
 * @returns
 * 0 if success, -1 if error.
 */
int spi_transfer(int spi_fd, uint8_t tx, uint8_t *rx);

#endif