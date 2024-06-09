#ifndef __RP3_H__
#define __RP3_H__

#define NO_RAIN_GPIO 14

struct DHTresult{
	float humi;
	float temp;
};

struct weatherResult {
  struct DHTresult DHT;
  char light; 
  int rain;
};

#endif