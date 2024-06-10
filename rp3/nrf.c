#include <stdio.h>
#include <unistd.h>
#include "gpio.h"
#include "spi.h"
#include "nrf.h"

int __nrf_write_reg(nrf_args *nrf, nrf_register reg, uint8_t byte)
{
    if (spi_transfer(nrf->spi_fd, W_REGISTER | reg, NULL) == -1 ||
        spi_transfer(nrf->spi_fd, byte, NULL) == -1)
    {
        perror("[__nrf_write_reg] spi_transfer");
        return -1;
    }
    return 0;
}

int __nrf_read_reg(nrf_args *nrf, nrf_register reg, uint8_t *byte)
{
    // if (spi_transfer(nrf->spi_fd, R_REGISTER | reg, byte))
    if (spi_transfer(nrf->spi_fd, R_REGISTER | reg, NULL) == -1 ||
        spi_transfer(nrf->spi_fd, NOP, byte) == -1)
    {
        perror("[__nrf_read_reg] spi_transfer");
        return -1;
    }
    return 0;
}

int __nrf_init(nrf_args *nrf)
{
    /* Initialize GPIO pins for CE and CSN */
    gpio_export(CE);
    gpio_export(CSN);
    gpio_set_direction(CE, OUT);
    gpio_set_direction(CSN, OUT);
    gpio_write(CE, LOW);
    gpio_write(CSN, HIGH);
    usleep(10000);
    /* Set payload width */
    SPI_ON();
    if (__nrf_write_reg(nrf, RX_PW_P0, PAYLOAD_WIDTH) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_init] RX_PW_P0");
        return -1;
    }
    /* Enable pipe 0, disable the others */
    if (__nrf_write_reg(nrf, EN_RXADDR, 1) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_init] EN_RXADDR");
        return -1;
    }
    /* Set RX address */
    if (spi_transfer(nrf->spi_fd, W_REGISTER | RX_ADDR_P0, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_init] RX_ADDR_P0");
        return -1;
    }
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        if (spi_transfer(nrf->spi_fd, nrf->address[i], NULL) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_init] RX_ADDR_P0");
            return -1;
        }
    }
    /* Set TX address */
    if (spi_transfer(nrf->spi_fd, W_REGISTER | TX_ADDR, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_init] TX_ADDR");
        return -1;
    }
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        if (spi_transfer(nrf->spi_fd, nrf->address[i], NULL) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_init] TX_ADDR");
            return -1;
        }
    }
    /* Enable CRC, 16 bits, powering up, with mode `nrf->mode` */
    if (__nrf_write_reg(nrf, CONFIG, 0b00001110 + nrf->mode) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_init] CONFIG");
        return -1;
    }
    /* Wait for powering up */
    usleep(1000);
    /* Check boot success */
    __nrf_dump_registers(nrf);
    SPI_OFF();
    return 0;
}

int __nrf_send(nrf_args *nrf)
{
    /* Test mode */
    if (nrf->mode == TX_MODE)
    {
        perror("[__nrf_send] Mode dismatch");
        return -1;
    }
    /* Flush TX FIFO */
    SPI_ON();
    if (spi_transfer(nrf->spi_fd, FLUSH_TX, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_send] FLUSH_TX");
        return -1;
    }
    /* Upload TX FIFO */
    if (spi_transfer(nrf->spi_fd, W_TX_PAYLOAD, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_send] W_TX_PAYLOAD");
        return -1;
    }
    for (int i = 0; i < PAYLOAD_WIDTH; i++)
    {
        if (spi_transfer(nrf->spi_fd, nrf->data[i], NULL) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_send] W_TX_PAYLOAD");
            return -1;
        }
    }
    SPI_OFF();
    /* Send! */
    gpio_write(CE, HIGH);
    usleep(10);
    gpio_write(CE, LOW);
    usleep(1000);
    /* Test send complete */
    uint8_t status;
    SPI_ON();
    do
    {
        if (__nrf_read_reg(nrf, STATUS, &status) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_send] Test send");
            return -1;
        }
        usleep(1000);
    } while (~status & TX_DS);
    /* Clear TX_DS bit */
    if (__nrf_write_reg(nrf, STATUS, TX_DS) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_send] Clear TX_DS");
        return -1;
    }
    SPI_OFF();
}

int __nrf_receive(nrf_args *nrf)
{
    /* Test mode */
    if (nrf->mode == RX_MODE)
    {
        perror("[__nrf_receive] Mode dismatch");
        return -1;
    }
    /* Flush RX FIFO */
    SPI_ON();
    if (spi_transfer(nrf->spi_fd, FLUSH_RX, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_receive] FLUSH_RX");
        return -1;
    }
    SPI_OFF();
    /* RX mode on */
    gpio_write(CE, HIGH);
    usleep(1000);
    /* Wait for data */
    uint8_t status;
    SPI_ON();
    do
    {
        if (__nrf_read_reg(nrf, STATUS, &status) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_receive] Test receive");
            return -1;
        }
        usleep(1000);
    } while (~status & RX_DR);
    /* Clear TX_DS bit */
    if (__nrf_write_reg(nrf, STATUS, RX_DR) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_send] Clear RX_DR");
        return -1;
    }
    SPI_OFF();
    gpio_write(CE, LOW);
    /* Download RX FIFO */
    SPI_ON();
    if (spi_transfer(nrf->spi_fd, R_RX_PAYLOAD, NULL) == -1)
    {
        __nrf_finalize(nrf);
        perror("[__nrf_receive] R_RX_PAYLOAD");
        return -1;
    }
    for (int i = 0; i < PAYLOAD_WIDTH; i++)
    {
        if (spi_transfer(nrf->spi_fd, NOP, &nrf->data[i]) == -1)
        {
            __nrf_finalize(nrf);
            perror("[__nrf_receive] R_RX_PAYLOAD");
            return -1;
        }
    }
    SPI_OFF();
    return 0;
}

void __nrf_finalize(nrf_args *nrf)
{
    /* Check problem */
    // __nrf_dump_registers(nrf);
    /* Powering down */
    __nrf_write_reg(nrf, CONFIG, 0);
    /* SPI OFF */
    gpio_write(CSN, HIGH);
    /* Wait for powering down */
    usleep(1000);
    gpio_unexport(CE);
    gpio_unexport(CSN);
}

void __nrf_dump_registers(nrf_args *nrf)
{
    uint8_t address[ADDR_LENGTH];
    uint8_t byte;
    printf("nRF14L01 Registers Value\n");
    __nrf_read_reg(nrf, CONFIG, &byte);
    printf("CONFIG : %02x\n", byte);
    __nrf_read_reg(nrf, EN_AA, &byte);
    printf("EN_AA : %02x\n", byte);
    __nrf_read_reg(nrf, EN_RXADDR, &byte);
    printf("EN_RXADDR : %02x\n", byte);
    __nrf_read_reg(nrf, SETUP_AW, &byte);
    printf("SETUP_AW : %02x\n", byte);
    __nrf_read_reg(nrf, SETUP_RETR, &byte);
    printf("SETUP_RETR : %02x\n", byte);
    __nrf_read_reg(nrf, RF_CH, &byte);
    printf("RF_CH : %02x\n", byte);
    __nrf_read_reg(nrf, RF_SETUP, &byte);
    printf("RF_SETUP : %02x\n", byte);
    __nrf_read_reg(nrf, STATUS, &byte);
    printf("STATUS : %02x\n", byte);
    printf("RX_ADDR_P0 : ");
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        __nrf_read_reg(nrf, RX_ADDR_P0, &byte);
        printf("%02x", byte);
    }
    printf("\n");
    printf("RX_ADDR_P1 : ");
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        __nrf_read_reg(nrf, RX_ADDR_P1, &byte);
        printf("%02x", byte);
    }
    printf("\n");
    __nrf_read_reg(nrf, RX_ADDR_P2, &byte);
    printf("RX_ADDR_P2 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_ADDR_P3, &byte);
    printf("RX_ADDR_P3 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_ADDR_P4, &byte);
    printf("RX_ADDR_P4 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_ADDR_P5, &byte);
    printf("RX_ADDR_P5 : %02x\n", byte);
    printf("TX_ADDR : ");
    for (int i = 0; i < ADDR_LENGTH; i++)
    {
        __nrf_read_reg(nrf, TX_ADDR, &byte);
        printf("%02x", byte);
    }
    printf("\n");
    __nrf_read_reg(nrf, RX_PW_P0, &byte);
    printf("RX_PWD_P0 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_PW_P1, &byte);
    printf("RX_PWD_P1 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_PW_P2, &byte);
    printf("RX_PWD_P2 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_PW_P3, &byte);
    printf("RX_PWD_P3 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_PW_P4, &byte);
    printf("RX_PWD_P4 : %02x\n", byte);
    __nrf_read_reg(nrf, RX_PW_P5, &byte);
    printf("RX_PWD_P5 : %02x\n", byte);
    __nrf_read_reg(nrf, FIFO_STATUS, &byte);
    printf("FIFO_STATUS: %02x\n", byte);
    __nrf_read_reg(nrf, DYNPD, &byte);
    printf("DYNPD : %02x\n", byte);
    __nrf_read_reg(nrf, FEATURE, &byte);
    printf("FEATURE : %02x\n", byte);
}