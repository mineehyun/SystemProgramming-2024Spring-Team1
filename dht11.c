#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "bcm2835.c"
#include <unistd.h>
#include "rp2.h"

#define MAXTIMINGS 100
#define AM2302 22
#define PIN 14

int readDHT(int pin);
 
int dht_function(struct DHTresult* dhtResult)
{
  if (!bcm2835_init())
        return 1;
  printf("Using pin #%d\n", PIN);
  readDHT(PIN);
  return 0;
}
int bits[250], data[100];
int bitidx = 0;

int readDHT(int pin) {
  int counter = 0;
  int laststate = HIGH;
  int j=0;
  // Set GPIO pin to output
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

  bcm2835_gpio_write(pin, HIGH);
  usleep(500000);  // 500 ms
  bcm2835_gpio_write(pin, LOW);
  usleep(20000);

  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // wait for pin to drop?
  while (bcm2835_gpio_lev(pin) == 1) {
    usleep(1);
  }

  // read data!
  for (int i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while ( bcm2835_gpio_lev(pin) == laststate) {
    counter++;
	  // usleep(1);		// overclocking might change this?
        if (counter == 1000) break;
    }
    laststate = bcm2835_gpio_lev(pin);
    if (counter == 1000) break;
    bits[bitidx++] = counter;
    if ((i>3) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > 200)
        data[j/8] |= 1;
      j++;
    }
  }

#ifdef DEBUG
  for (int i=3; i<bitidx; i+=2) {
    printf("bit %d: %d\n", i-3, bits[i]);
    printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > 200);
  }
#endif

  printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);

  if ((j >= 39) &&
      (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
    printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);
    return 1;
  }
  else if (j==39){
    printf("39\n");
    j++;
    data[j/8] <<= 1;
    data[j/8] |= 1;
    printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);
  }
  return 0;
}