#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dht.c"

struct DHTresult{
	float humi;
	float temp;
};

void* dht_function(void* dhtResult){
	printf("dht_fine\n");
	return dhtResult;
}

