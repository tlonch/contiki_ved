#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "uip.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h" 

#include "nrf_saadc.h"
#include "nrf_drv_saadc.h"
//#include "cfs/cfs.h" 

#define APP_ERROR_CHECK(X) if(X!=0) printf("error: %d \n", X);
#define SAMPLES_IN_BUFFER 1
#define NUM_CHAN 3
#define NUM_DATA 100

double dataCollection[NUM_DATA];

static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state
                                          == ADDR_PREFERRED)) {
      PRINTF("  ");
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
      if(state == ADDR_TENTATIVE) {
        uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}


static nrf_saadc_value_t       m_buffer_pool[2][SAMPLES_IN_BUFFER];

nrf_drv_saadc_event_handler_t saadc_callback(nrf_drv_saadc_evt_t* p_event)
{
printf("callback\n");
return 0;
} 
 
int saadc_get_value(short int chan)
{
    int err_code;
    int value;
    nrf_saadc_channel_config_t channel_0_config;
        channel_0_config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
        channel_0_config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
        channel_0_config.gain       = NRF_SAADC_GAIN1_6;
        channel_0_config.reference  = NRF_SAADC_REFERENCE_INTERNAL;
        channel_0_config.acq_time   = NRF_SAADC_ACQTIME_10US;
        channel_0_config.mode       = NRF_SAADC_MODE_SINGLE_ENDED;
        channel_0_config.pin_p      = NRF_SAADC_INPUT_AIN0;
        channel_0_config.pin_n      = NRF_SAADC_INPUT_DISABLED;
      nrf_saadc_channel_config_t channel_1_config;
        channel_1_config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
        channel_1_config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
        channel_1_config.gain       = NRF_SAADC_GAIN1_6;
        channel_1_config.reference  = NRF_SAADC_REFERENCE_INTERNAL;
        channel_1_config.acq_time   = NRF_SAADC_ACQTIME_10US;
        channel_1_config.mode       = NRF_SAADC_MODE_SINGLE_ENDED;
        channel_1_config.pin_p      = NRF_SAADC_INPUT_AIN1; // PIN P0.03
        channel_1_config.pin_n      = NRF_SAADC_INPUT_DISABLED;
      nrf_saadc_channel_config_t channel_2_config;
        channel_2_config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
        channel_2_config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
        channel_2_config.gain       = NRF_SAADC_GAIN1_6;
        channel_2_config.reference  = NRF_SAADC_REFERENCE_INTERNAL;
        channel_2_config.acq_time   = NRF_SAADC_ACQTIME_10US;
        channel_2_config.mode       = NRF_SAADC_MODE_SINGLE_ENDED;
        channel_2_config.pin_p      = NRF_SAADC_INPUT_AIN2; // PIN P0.04
        channel_2_config.pin_n      = NRF_SAADC_INPUT_DISABLED;
    
        if(chan==0)
        {
          err_code = nrf_drv_saadc_channel_init(chan, &channel_0_config);
          APP_ERROR_CHECK(err_code);
        
          err_code = nrf_drv_saadc_sample_convert   (chan,&m_buffer_pool[0][0]);  
          APP_ERROR_CHECK(err_code);
        }
        if(chan==1)
        {
          err_code = nrf_drv_saadc_channel_init(chan, &channel_1_config);
          APP_ERROR_CHECK(err_code);
        
          err_code = nrf_drv_saadc_sample_convert   (chan,&m_buffer_pool[0][0]);  
          APP_ERROR_CHECK(err_code);
        }
        if(chan==2)
        {
          err_code = nrf_drv_saadc_channel_init(chan, &channel_2_config);
          APP_ERROR_CHECK(err_code);
        
          err_code = nrf_drv_saadc_sample_convert   (chan,&m_buffer_pool[0][0]);  
          APP_ERROR_CHECK(err_code);
        }
    

    return m_buffer_pool[0][0];
}

/*void writeToFile(double value, char* filename)
{
  char s[10];
  sprintf(s, "%lf", value);
  int fd_write;
  int n;
  fd_write = cfs_open(filename, CFS_WRITE | CFS_APPEND);
  if(fd_write != -1) {
    n = cfs_write(fd_write, s, sizeof(s));
    cfs_close(fd_write);
    printf("step 4: successfully appended data to cfs. wrote %i bytes  \n",n);
  } else {
    printf("ERROR: could not write to memory in step 4.\n");
  }
}
*/



static double saadcToDouble()
{
  return (double) saadc_get_value(0);
}

static char* voltageSensor(){
  char* s;
  return  sprintf(s, "%lf", saadcToDouble()*3.3/1024);
}
static double tempSensor(){
  return saadc_get_value(1)*200/1024;
}

static double speedSensor(){
  return saadc_get_value(2);
}

PROCESS(er_example_server, "nRF52 DK Data Collection");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  static int chan = 0;
  static struct etimer timer;

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  etimer_set(&timer, CLOCK_SECOND);

  PRINTF("Starting Erbium Example Server\n");

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  print_local_addresses();
  nrf_drv_saadc_init(NULL, saadc_callback);

  /* Define application-specific events here. */
  while (1) {
    
    PROCESS_WAIT_EVENT();
    int i=1;
    
    if (ev == PROCESS_EVENT_TIMER){
	      //printf("CHAN %d : %f \n", chan, voltageSensor());
        if(chan==0){
          printf("CHAN %d : %s \n", chan, voltageSensor());
          //dataCollection[i]=voltageSensor();
          //writeToFile(voltageSensor(), "datas");
        } 
        else if(chan==1)
        {
          printf("CHAN %d : %f \n", chan, tempSensor());
          dataCollection[i]=tempSensor();
          //writeToFile(tempSensor(), "datas");
        }    
        else if(chan==2)
        {
          printf("CHAN %d : %f \n", chan, speedSensor());
          dataCollection[i]=speedSensor();
          //writeToFile(speedSensor(), "datas");
        }
        i++;
	      if (++chan >= NUM_CHAN) chan = 0;
  
        etimer_reset(&timer);
    }

    
  }


  PROCESS_END();
}

/**
 * @}
 * @}
 */
