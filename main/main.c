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

    esp_err_t err;

    static uint8_t can_test_data[8] =
    {
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08
    };

    /*Initialize ADC*/
    err = cem_adc_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC initialization failed");
    }
    else
    {
        ESP_LOGI(TAG, "ADC initialization successful");
    }
    /*Initialize CAN*/
    err = cem_can_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "CAN initialization failed");
    }
    else
    {
        ESP_LOGI(TAG, "CAN initialization successful");
    }

     /*
     * Send one CAN test frame
     */

    if (err == ESP_OK)
    {
        err = cem_can_transmit(
            0x600,
            can_test_data,
            8
        );

        if (err != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "CAN test transmit failed: %s",
                esp_err_to_name(err)
            );
        }
        else
        {
            ESP_LOGI(TAG, "CAN test frame queued");
        }
    }

    /*Main ADC monitoring loop*/
    while (1)
{
    int adc_mv[CEM_ADC_COUNT] = {0};

    /*
     * Read all eight CEM analog inputs.
     *
     * Physical J2 mapping:
     * ADC1 = J2.10
     * ADC2 = J2.11
     * ADC3 = J2.4
     * ADC4 = J2.3
     * ADC5 = J2.9
     * ADC6 = J2.8
     * ADC7 = J2.2
     * ADC8 = J2.7
     */
    for (int channel = 0; channel < CEM_ADC_COUNT; channel++)
    {
        err = cem_adc_read(
            (cem_adc_channel_t)channel,
            &adc_mv[channel]
        );

        if (err != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "ADC%d read failed: %s",
                channel + 1,
                esp_err_to_name(err)
            );
        }
    }

    /*
     * Display all eight ADC channels once per second.
     * Values are reported in millivolts.
     */
    ESP_LOGI(
        TAG,
        "ADC1=%4d mV | ADC2=%4d mV | ADC3=%4d mV | ADC4=%4d mV | "
        "ADC5=%4d mV | ADC6=%4d mV | ADC7=%4d mV | ADC8=%4d mV",
        adc_mv[CEM_ADC_1],
        adc_mv[CEM_ADC_2],
        adc_mv[CEM_ADC_3],
        adc_mv[CEM_ADC_4],
        adc_mv[CEM_ADC_5],
        adc_mv[CEM_ADC_6],
        adc_mv[CEM_ADC_7],
        adc_mv[CEM_ADC_8]
    );

    vTaskDelay(pdMS_TO_TICKS(1000));
}
}