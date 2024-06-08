#ifndef __NRF_H_
#define __NRF_H_

#include <stdint.h>

/* GPIO pin definition */
#define CE 25
#define CSN 8
/* User define configurations */
#define ADDR_LENGTH 5
#define PAYLOAD_WIDTH 32
/* SPI command definition */
#define R_REGISTER 0b00000000
#define W_REGISTER 0b00100000
#define R_RX_PAYLOAD 0b01100001
#define W_TX_PAYLOAD 0b10100000
#define FLUSH_TX 0b11100001
#define FLUSH_RX 0b11100010
#define NOP 0b11111111 // no operation
/* Registers definition */
#define CONFIG 0x00
#define STATUS 0x07
#define RX_ADDR_P0 0x0A
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
/* STATUS register condition check bit masks */
#define TX_DS 1 << 5 // this bit means send done
#define RX_DR 1 << 6 // this bit means receive done
/* Useful macros */
#define SPI_ON GPIOWrite(CSN, LOW) // Enables SPI communication
#define SPI_OFF GPIOWrite(CSN, HIGH) // Disables SPI communication

void __nrf_write_reg(int spi_fd, uint8_t reg, uint8_t value);

uint8_t __nrf_read_reg(int spi_fd, uint8_t reg);

/**
 * @brief
 * Boot nRF24L01 and set PRIM_RX to `mode`.
 * 
 * @param spi_fd
 * File descriptor of SPI device.
 * @param mode
 * Must set to be TX(0) or RX(1) mode.
 */
void __nrf_init(int spi_fd, uint8_t mode);

/**
 * @brief
 * Set TX address to `addr`
 */
void __nrf_tx_addr(int spi_fd, uint8_t addr[]);

/**
 * @brief
 * Set RX address to `addr`
 */
void __nrf_rx_addr(int spi_fd, uint8_t addr[]);

/**
 * @brief
 * Send up to 32 bytes of datas through nRF24L01.
 * 
 * @param spi_fd
 * File descriptor of SPI device.
 * @param addr
 * 32-bit address.
 * @param data
 * Data buffer.
 */
void __nrf_send(int spi_fd, uint8_t data[], int length);

/**
 * @brief
 * Receive up to 32 bytes of datas from nRF24L01.
 * 
 * @param spi_fd
 * File descriptor of SPI device.
 * @param addr
 * 32-bit address.
 * @param mode
 * Data buffer.
 */
void __nrf_receive(int spi_fd, uint8_t data[], int length);

void __nrf_shutdown(int spi_fd);

#endif