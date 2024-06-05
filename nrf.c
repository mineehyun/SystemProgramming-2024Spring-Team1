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
    char *mode_strs[2] = { "TX(send)", "RX(receive)" };
    printf("[nRF init] Initializing module in %s mode\n", mode_strs[mode]);
    if (mode != TX_MODE || mode != RX_MODE)
    {
        fprintf(stderr, "[nRF init] @mode must be TX_MODE(0) or RX_MODE(1)\n");
        return;
    }
    /* Init GPIO */
    GPIOExport(CE);
    GPIOExport(CSN);
    GPIODirection(CE, OUT);
    GPIODirection(CSN, OUT);
    /* Config nRF24L01 */
    SPI_ON;
    nrf_write_reg(spi_fd, RX_PW_P0, PAYLOAD_WIDTH); // Set payload width to 32 bytes
    nrf_write_reg(spi_fd, CONFIG, 0b1010 + mode); // CRC enable(<< 3) with 8 bits config(<< 2), Powering on(<< 1), tx/rx mode set(<< 0)
    usleep(1500);
    SPI_OFF;
    printf("[nRF init] Initialized module in %s mode\n", mode_strs[mode]);
}

void __nrf_send(int spi_fd, uint8_t addr[], uint8_t data[], int length)
{
    printf("[nRF send] Trying to send data\n");
    SPI_ON;
    /* Get CONFIG register's first bit to Check mode */
    if (nrf_read_reg(spi_fd, CONFIG) & 1)
    {
        fprintf(stderr, "[nRF send] nRF24L01 is in RX(receive) mode\n");
        SPI_OFF;
        return;
    }
    /* Upload address */
    spi_transfer(spi_fd, W_REGISTER | TX_ADDR);
    for (int i = 0; i < 5; i++)
    {
        spi_transfer(spi_fd, addr[i]);
    }
    /* Load data to TX FIFO */
    spi_transfer(spi_fd, W_TX_PAYLOAD);
    for (int i = 0; i < length; i++)
    {
        spi_transfer(spi_fd, data[i]);
    }
    SPI_OFF;
    /* SEND! */
    GPIOWrite(CE, HIGH);
    usleep(10);
    GPIOWrite(CE, LOW);
    usleep(130);
    printf("[nRF send] Data sent\n");
}

void __nrf_receive(int spi_fd, uint8_t addr[], uint8_t data[], int length)
{
    printf("[nRF receive] Waiting for data\n");
    SPI_ON;
    /* Get CONFIG register's first bit to check mode */
    if (~nrf_read_reg(spi_fd, CONFIG) & 1)
    {
        fprintf(stderr, "[nRF send] nRF24L01 is in TX(send) mode\n");
        SPI_OFF;
        return;
    }
    /* Upload address */
    spi_transfer(spi_fd, W_REGISTER | RX_ADDR_P0);
    for (int i = 0; i < 5; i++)
    {
        spi_transfer(spi_fd, addr[i]);
    }
    /* Flush RX FIFO */
    spi_transfer(spi_fd, FLUSH_RX);
    SPI_OFF;
    /* Enter RX mode */
    GPIOWrite(CE, HIGH);
    usleep(130);
    SPI_ON;
    while (nrf_read_reg(spi_fd, STATUS) & RX_DR) usleep(1000); // Wait for data
    SPI_OFF;
    /* Data received. Exit RX mode */
    GPIOWrite(CE, LOW);
    /* Download data from RX FIFO */
    SPI_ON;
    spi_transfer(spi_fd, R_RX_PAYLOAD);
    for (int i = 0; i < length; i++)
    {
        data[i] = spi_transfer(spi_fd, NOP);
    }
    SPI_OFF;
    printf("[nRF receive] Data received\n");
}