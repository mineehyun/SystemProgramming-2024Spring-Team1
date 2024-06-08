#ifndef __NRF_H_
#define __NRF_H_

#include <pthread.h>
#include <stdint.h>

/* GPIO pin definition */
#define CE 27
#define CSN 22
/* Default configs */
#define ADDR_LENGTH 5
#define PAYLOAD_WIDTH 32
/* SPI command definition */
typedef enum
{
    R_REGISTER = 0b00000000, // LSB 5 bits: register
    W_REGISTER = 0b00100000, // LSB 5 bits: register
    R_RX_PAYLOAD = 0b01100001,
    W_TX_PAYLOAD = 0b10100000,
    FLUSH_TX = 0b11100001,
    FLUSH_RX = 0b11100010,
    REUSE_TX_PL = 0b11100011,
    ACTIVATE = 0b01010000,
    R_RX_PL_WID = 0b01100000,
    W_ACK_PAYLOAD = 0b10101000, // LSB 3 bits: pipe number
    NOP = 0b11111111,
} nrf_command;
/* Registers definition */
typedef enum
{
    CONFIG = 0x00,
    EN_AA = 0x01,
    EN_RXADDR = 0x02,
    SETUP_AW = 0x03,
    SETUP_RETR = 0x04,
    RF_CH = 0x05,
    RF_SETUP = 0x06,
    STATUS = 0x07,
    OBSERVE_TX = 0x08,
    CD = 0x09,
    RX_ADDR_P0 = 0x0a,
    RX_ADDR_P1 = 0x0b,
    RX_ADDR_P2 = 0x0c,
    RX_ADDR_P3 = 0x0d,
    RX_ADDR_P4 = 0x0e,
    RX_ADDR_P5 = 0x0f,
    TX_ADDR = 0x10,
    RX_PW_P0 = 0x11,
    RX_PW_P1 = 0x12,
    RX_PW_P2 = 0x13,
    RX_PW_P3 = 0x14,
    RX_PW_P4 = 0x15,
    RX_PW_P5 = 0x16,
    FIFO_STATUS = 0x17,
    DYNPD = 0x1c,
    FEATURE = 0x1d,
} nrf_register;
/* STATUS register condition check bit masks */
#define RX_DR 1 << 6 // this bit means receive done
#define TX_DS 1 << 5 // this bit means send done
/* Modes */
typedef enum
{
    TX_MODE = 0,
    RX_MODE = 1,
} nrf_mode;
/* CSN pin setting macro; must be used in a pair */
#define SPI_ON() \
    do           \
    {            \
    gpio_write(CSN, LOW)
#define SPI_OFF()          \
    gpio_write(CSN, HIGH); \
    }                      \
    while (0)

typedef struct
{
    int spi_fd;
    nrf_mode mode;
    uint8_t address[ADDR_LENGTH];
    uint8_t data[PAYLOAD_WIDTH];
} nrf_args;

/**
 * Write single `byte` to `reg`. Do not manipulate CSN pin.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int __nrf_write_reg(nrf_args *nrf, nrf_register reg, uint8_t byte);

/**
 * Read single `*byte` from `reg`. Do not manipulate CSN pin.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int __nrf_read_reg(nrf_args *nrf, nrf_register reg, uint8_t *byte);

/**
 * Init GPIOs and power nRF24L01 on. Sets address also.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int __nrf_init(nrf_args *nrf);

/**
 * Send `nrf->data` and wait for ACK signal.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int __nrf_send(nrf_args *nrf);

/**
 * Wait for data and download into `nrf->data`.
 *
 * @returns
 * 0 if success, -1 if error.
 */
int __nrf_receive(nrf_args *nrf);

/**
 * Unexport GPIOs and shutdown nRF24L01. Assuming CSN LOW.
 */
void __nrf_finalize(nrf_args *nrf);

/**
 * Used on debugging. Do not manipulate CSN pin.
 */
void __nrf_dump_registers(nrf_args *nrf);

#endif