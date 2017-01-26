#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "uip.h"
#include "platform/nrf52dk/dev/button-sensor.h"//librairie du bouton
#include "platform/nrf52dk/dev/temperature-sensor.h" //librairie du capteur température de la carte

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h" 

#include "nrf_saadc.h"
#include "nrf_drv_saadc.h"

float  a = 11222.2;
void main()
{
	printf("%ff\n", a);
}
