#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

#define SPI_PATH "/dev/spidev0.0"
#define SPI_SPEED 1000000
#define SPI_MODE SPI_MODE_0
#define SPI_BITS_PER_WORD 8

/**
 * spi_init()
 * 
 * DESCRIPTION
 *  Open SPI device and initialize it.
 *  Must close fd returned.
 * 
 * RETURN
 *  SPI file descriptor. -1 if error.
*/
int spi_init();

/**
 * spi_transfer(@tx)
 * 
 * DESCRIPTION
 *  Send a byte of data @tx to SPI device,
 *  and receive a byte of data `rx` from SPI device.
 * 
 * RETURN
 *  A byte of data received. 0 if error.
*/
uint8_t spi_transfer(int spi_fd, uint8_t tx);

#endif