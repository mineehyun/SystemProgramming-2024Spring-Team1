#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include "gas.h"

extern int gas_level;  // Shared variable for the gas level
extern int gas_threshold;
extern pthread_mutex_t lock;  // Mutex for synchronization

#define DEVICE "/dev/spidev0.0"
static uint8_t MODE = 0;
static uint8_t BITS = 8;
static uint32_t CLOCK = 1000000;
static uint16_t DELAY = 5;

// SPI sample code
static int prepare(int fd) {
  if (ioctl(fd, SPI_IOC_WR_MODE, &MODE) == -1) {
    perror("Can't set MODE");
    return -1;
  }
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &BITS) == -1) {
    perror("Can't set number of BITS");
    return -1;
  }
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &CLOCK) == -1) {
    perror("Can't set write CLOCK");
    return -1;
  }
  if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &CLOCK) == -1) {
    perror("Can't set read CLOCK");
    return -1;
  }
  return 0;
}

uint8_t control_bits_differential(uint8_t channel) {
  return (channel & 7) << 4;
}

uint8_t control_bits(uint8_t channel) {
  return 0x8 | control_bits_differential(channel);
}

int readadc(int fd, uint8_t channel) {
  uint8_t tx[] = {1, control_bits(channel), 0};
  uint8_t rx[3];
  struct spi_ioc_transfer tr = {
      .tx_buf = (unsigned long)tx,
      .rx_buf = (unsigned long)rx,
      .len = 3,
      .delay_usecs = DELAY,
      .speed_hz = CLOCK,
      .bits_per_word = BITS,
  };
  if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) == 1) {
    perror("IO Error");
    abort();
  }
  return ((rx[1] << 8) & 0x300) | (rx[2] & 0xFF);
}

void *gas_function(void *vargp) {
    // init spi device
    int spi_fd = open(DEVICE, O_RDWR);
    if (spi_fd <= 0)
    {
        perror("Device open error");
        return NULL;
    }
    if (prepare(spi_fd) == -1)
    {
        perror("Device prepare error");
        return NULL;
    }
    // 부저조용해
    pwm_export(0);
    pwm_unexport(0);
    // gas detection
    int local_gas_level;
    while (1) {
        local_gas_level = readadc(spi_fd, 0);
        pthread_mutex_lock(&lock);
        gas_level = local_gas_level;  // Update the shared variable within a lock
        pthread_mutex_unlock(&lock);
        if (local_gas_level > gas_threshold)
        {
          pthread_t pt;
          siren_thread_args args =
          {
            .freq_min = 500,
            .freq_max = 1000,
            .pwm_num = 0,
          };
          pthread_create(&pt, NULL, siren_thread, &args);
          pthread_join(pt, NULL);
        }
        printf("Current gas value: %d\n", gas_level);
        usleep(100000);
    }
    return NULL;
}
