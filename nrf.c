#include <stdio.h>
#include "gpio.h"
#include "spi.h"
#include "nrf.h"

void __nrf_write_reg(int spi_fd, uint8_t reg, uint8_t value)
{
    spi_transfer(spi_fd, W_REGISTER | reg);
    spi_transfer(spi_fd, value);
}

uint8_t __nrf_read_reg(int spi_fd, uint8_t reg)
{
    spi_transfer(spi_fd, R_REGISTER | reg);
    return spi_transfer(spi_fd, NOP);
}

void __nrf_init(int spi_fd, uint8_t mode)
{
    GPIOExport(CE);
    GPIOExport(CSN);
    GPIODirection(CE, OUT);
    GPIODirection(CSN, OUT);
    /* Config nRF24L01 */
    SPI_ON;
    __nrf_write_reg(spi_fd, RX_PW_P0, PAYLOAD_WIDTH); // Set payload width
    __nrf_write_reg(spi_fd, CONFIG, 0b1010 + mode);   // CRC enable(<< 3) with 8 bits config(<< 2), Powering on(<< 1), tx/rx mode set(<< 0)
    usleep(1500);
    SPI_OFF;
}

void __nrf_set_mode_tx(int spi_fd)
{
    SPI_ON;
    uint8_t __config = __nrf_read_reg(spi_fd, CONFIG);
    __config &= ~(1 << 0); // Clear PRIM_RX bit
    __nrf_write_reg(spi_fd, CONFIG, __config);
    SPI_OFF;
}

void __nrf_set_mode_rx(int spi_fd)
{
    SPI_ON;
    uint8_t __config = __nrf_read_reg(spi_fd, CONFIG);
    __config |= (1 << 0); // Set PRIM_RX bit
    __nrf_write_reg(spi_fd, CONFIG, __config);
    SPI_OFF;
}

void __nrf_addr(int spi_fd, uint8_t addr[])
{
    SPI_ON;
    spi_transfer(spi_fd, W_REGISTER | TX_ADDR);
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        spi_transfer(spi_fd, addr[i]);
    }
    spi_transfer(spi_fd, W_REGISTER | RX_ADDR_P0);
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        spi_transfer(spi_fd, addr[i]);
    }
    SPI_OFF;
}

void __nrf_send(int spi_fd, uint8_t data[], int length)
{
    SPI_ON;
    /* Upload data */
    spi_transfer(spi_fd, W_TX_PAYLOAD);
    for (int i = 0; i < length; i++)
    {
        spi_transfer(spi_fd, data[i]);
    }
    SPI_OFF;
    /* Send! */
    GPIOWrite(CE, HIGH);
    usleep(10);
    GPIOWrite(CE, LOW);
    usleep(130);
    /* Wait for send complete (TX_DS == 1) */
    SPI_ON;
    while (~__nrf_read_reg(spi_fd, STATUS) & TX_DS)
        usleep(1000);
    /* Clear TX_DS bit */
    __nrf_write_reg(spi_fd, STATUS, (1 << TX_DS));
    SPI_OFF;
}

void __nrf_receive(int spi_fd, uint8_t data[], int length)
{
    SPI_ON;
    /* Flush RX FIFO */
    spi_transfer(spi_fd, FLUSH_RX);
    SPI_OFF;
    /* Enter RX mode */
    GPIOWrite(CE, HIGH);
    usleep(130);
    /* Wait for data (RX_DR == 1) */
    SPI_ON;
    while (~__nrf_read_reg(spi_fd, STATUS) & RX_DR)
        usleep(1000);
    /* Clear RX_DR bit */
    __nrf_write_reg(spi_fd, STATUS, (1 << RX_DR));
    SPI_OFF;
    /* Exit RX mode */
    GPIOWrite(CE, LOW);
    /* Download data */
    SPI_ON;
    spi_transfer(spi_fd, R_RX_PAYLOAD);
    for (int i = 0; i < length; i++)
    {
        data[i] = spi_transfer(spi_fd, NOP);
    }
    SPI_OFF;
}

void __nrf_shutdown(int spi_fd)
{
    SPI_ON;
    __nrf_write_reg(spi_fd, CONFIG, 0);
    SPI_OFF;
    GPIOUnexport(CE);
    GPIOUnexport(CSN);
}