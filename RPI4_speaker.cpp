#include <stdio.h>
#include <errno.h>
#include <string.h>
 
#include <wiringPi.h>
#include <softTone.h>
 
#define PIN 28

int scale [35] = {262, 0, 262, 0, 392, 392, 330, 330, 0, 0, 262, 0, 262, 0, 392, 392, 330, 330, 0, 0, 440, 440, 0, 392, 349, 349, 330, 330, 294, 294, 330, 330, 349, 349  } ;
 
int main ()
{
  int i ;
  wiringPiSetup () ;
  softToneCreate (PIN) ;
  for (i = 0 ; i < 35; ++i)
    {
      printf ("%3d\n", i) ;
      softToneWrite (PIN, scale [i]) ;
      delay (100) ;
    }
}

