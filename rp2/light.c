#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "spiex.c"

void* light_function(void* light) {
  int fd = open(DEVICE, O_RDWR);
  if (fd <= 0) {
    perror("Device open error");
    // return -1;
  }

  if (prepare(fd) == -1) {
    perror("Device prepare error");
    // return -1;
  }
  int value = readadc(fd, 0);
  int* light_ptr = (int*)light;
  light_ptr = (int*)light;
  *light_ptr= value;

  return NULL;
}