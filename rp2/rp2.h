#ifndef __RP2_H__
#define __RP2_H__


typedef enum
{
  DRY = 0,
  WET = 1,
} rain_state;

struct DHTresult
{
  int humi;
  int temp;
};

struct weatherResult
{
  struct DHTresult DHT;
  int light;
  rain_state rain;
};

#endif