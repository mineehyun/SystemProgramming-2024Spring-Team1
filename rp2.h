#ifndef __RP2_H__
#define __RP2_H__

#define NO_RAIN_GPIO 14

struct DHTresult{
	int humi;
	int temp;
};

struct weatherResult {
  struct DHTresult DHT;
  char light; 
  int rain;
};

#endif