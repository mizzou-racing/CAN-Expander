#pragma once

#include "esp_err.h"
#include "stdint.h"

/*
    CEM Analog Input Channels:
*/
typedef enum
{
    CEM_ADC_1 = 0,
    CEM_ADC_2,
    CEM_ADC_3,
    CEM_ADC_4,
    CEM_ADC_5,
    CEM_ADC_6,
    CEM_ADC_7,
    CEM_ADC_8,

    CEM_ADC_COUNT
}
cem_adc_channel_t;

/*
    Initalize ESP32-S3 ADC units and all eight CEM ADC units
*/

esp_err_t cem_adc_init(void);


esp_err_t cem_adc_read(
    cem_adc_channel_t channel, 
    int *millivolts
);