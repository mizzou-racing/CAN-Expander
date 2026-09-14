#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "esp_log.h"

#include "cem_pins.h"
#include "cem_adc.h"
#include "cem_can.h"

static const char *TAG = "CEM_EV";

void app_main(void)
{
    ESP_LOGI(TAG, "CEM EV firmware starting...");

    esp_err_t err = cem_adc_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC initialization failed");
    }
    else
    {
        ESP_LOGI(TAG, "ADC initialization successful");
    }
    
    err = cem_can_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "CAN initialization failed");
    }
    else
    {
        ESP_LOGI(TAG, "CAN initialization successful");
    }

    while(1)
    {
        int adc1_mv = 0;

        err = cem_adc_read(CEM_ADC_1, &adc1_mv);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "ADC1 = %d mV", adc1_mv);
        }
        else
        {
            ESP_LOGE(TAG, "ADC1 read failed: %s", esp_err_to_name(err));
        }

    }


    vTaskDelay(pdMS_TO_TICKS(1000));
}