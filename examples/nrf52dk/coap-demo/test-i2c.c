#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "/home/tim/Bureau/contiki/cpu/nrf52832/nrf-sdk/components/drivers_nrf/twi_master/nrf_drv_twi.h"
#include "nrf_delay.h"

/* ID du master TWI */
#define TWI_INSTANCE_ID     0

/* Adresse des capteurs */
//#define 

/* Indique si l'opération sur le TWI est terminée */
static volatile bool transfer_done = false;

/* On crée le master */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* valeure lue des sensor */
static uint8_t m_sample;

void data_handler(uint8_t temp)
{
    PRINTF("%d", temp);
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
       .scl                = //SCL PIN,
       .sda                = //SDA PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };
	
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
	

    nrf_drv_twi_enable(&m_twi);
}

/* Lecture des données du sensor */
static void read_sensor_data()
{
    transfer_done = false;

    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, /*Adresse du capteur*/, &m_sample, sizeof(m_sample));
    APP_ERROR_CHECK(err_code);

}

int main(void)
{
    twi_init();

    while (true)
    {
        nrf_delay_ms(500);

        do
        {
            __WFE();
        }while (transfer_done == false);

        read_sensor_data();
    }
}

// PART 2



PROCESS(er_example_i2c, "nRF52 DK I2C Sensor");
AUTOSTART_PROCESSES(&er_example_i2c);

PROCESS_THREAD(er_example_i2c, ev, data)
{
	twi_init();
	PROCESS_BEGIN();
	PROCESS_PAUSE();
	while (1) {
    		read_sensor_data();
  	}

  	PROCESS_END();
}
