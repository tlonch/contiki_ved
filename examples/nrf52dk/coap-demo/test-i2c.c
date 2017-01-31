#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "/home/dahmani/contiki_ved/cpu/nrf52832/nrf-sdk/components/drivers_nrf/twi_master/nrf_drv_twi.c"
#include "/home/dahmani/contiki_ved/cpu/nrf52832/nrf-sdk/components/drivers_nrf/twi_master/nrf_drv_twi.h"
#include "nrf_delay.h"


#include "contiki.h"
#include "contiki-net.h"
#include "uip.h"
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h" 


	/* ID du master TWI */
#define TWI_INSTANCE_ID     0

	/* Adresse des capteurs */
#define SHT7_MEAS_TEMP 00011

	/* Indique si l'opération sur le TWI est terminée */
static volatile bool transfer_done = false;

	/* On crée le master */
nrf_drv_twi_t m_twi = {
    .p_reg    = (void *)0x40003000,       /**< Pointer to the instance register set. */
    .irq       =   SPI0_TWI0_IRQn,         /**< Instance IRQ ID. */
    .instance_id= 0
};
//static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
	/* valeure lue des sensor */
static uint8_t m_sample;

void data_handler(uint8_t temp)
{
    PRINTF("la température est de %d degrés", temp);
}

void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_RX_DONE:
            data_handler(m_sample);
            transfer_done = true;
            break;
        default:
            break;
    }
}

	/* Initialisation du TWI */
void twi_init (void)
{
    ret_code_t err_code;
	
    const nrf_drv_twi_config_t twi_config = {
       .scl                = 27,
       .sda                = 26,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
    };
	
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
	
	    nrf_drv_twi_enable(&m_twi);
}
	/* Lecture des données du sensor */
static void read_sensor_data()
{
    transfer_done = false;
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, SHT7_MEAS_TEMP, &m_sample, sizeof(m_sample), false);
    APP_ERROR_CHECK(err_code);

}





PROCESS(er_example_twi, "TWI example");

AUTOSTART_PROCESSES(&er_example_twi);

PROCESS_THREAD(er_example_twi, ev, data){
	twi_init();
	PROCESS_BEGIN();
		PROCESS_PAUSE();

	while(1){
		read_sensor_data();
		PROCESS_END();
	}
}
