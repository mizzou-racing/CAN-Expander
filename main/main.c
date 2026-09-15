#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "cem_adc.h"
#include "cem_can.h"


static const char *TAG = "CEM_EV";


/*
 * Pack a 16-bit ADC millivolt value into two CAN bytes.
 *
 * Little-endian:
 *   byte[0] = low byte
 *   byte[1] = high byte
 */
static void pack_adc_value(
    uint8_t *data,
    int position,
    int millivolts
)
{
    data[position] =
        (uint8_t)(millivolts & 0xFF);

    data[position + 1] =
        (uint8_t)((millivolts >> 8) & 0xFF);
}


void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "CEM EV firmware starting...");


    /* =========================================================
     * Initialize ADC subsystem
     * =========================================================
     */

    err = cem_adc_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "ADC initialization failed: %s",
            esp_err_to_name(err)
        );

        return;
    }

    ESP_LOGI(TAG, "ADC initialization successful");


    /* =========================================================
     * Initialize CAN subsystem
     * =========================================================
     */

    err = cem_can_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "CAN initialization failed: %s",
            esp_err_to_name(err)
        );

        return;
    }

    ESP_LOGI(TAG, "CAN initialization successful");


    /* =========================================================
     * CAN transmit buffers
     *
     * 0x600:
     * ADC1 ADC2 ADC3 ADC4
     *
     * 0x601:
     * ADC5 ADC6 ADC7 ADC8
     * =========================================================
     */

    static uint8_t can_data_600[8] = {0};
    static uint8_t can_data_601[8] = {0};


    /* =========================================================
     * ADC storage
     * =========================================================
     */

    int adc_mv[CEM_ADC_COUNT] = {0};


    /* =========================================================
     * Main loop
     * =========================================================
     */

    while (1)
    {
        /*
         * -----------------------------------------------------
         * Read ADC1 through ADC8
         * -----------------------------------------------------
         */

        for (int i = 0; i < CEM_ADC_COUNT; i++)
        {
            err = cem_adc_read(
                (cem_adc_channel_t)i,
                &adc_mv[i]
            );

            if (err != ESP_OK)
            {
                ESP_LOGE(
                    TAG,
                    "ADC%d read failed: %s",
                    i + 1,
                    esp_err_to_name(err)
                );

                /*
                 * Set failed channel to zero for this test.
                 */
                adc_mv[i] = 0;
            }
        }


        /*
         * -----------------------------------------------------
         * Display ADC readings
         * -----------------------------------------------------
         */

        ESP_LOGI(
            TAG,
            "ADC1=%d mV  ADC2=%d mV  ADC3=%d mV  ADC4=%d mV",
            adc_mv[CEM_ADC_1],
            adc_mv[CEM_ADC_2],
            adc_mv[CEM_ADC_3],
            adc_mv[CEM_ADC_4]
        );

        ESP_LOGI(
            TAG,
            "ADC5=%d mV  ADC6=%d mV  ADC7=%d mV  ADC8=%d mV",
            adc_mv[CEM_ADC_5],
            adc_mv[CEM_ADC_6],
            adc_mv[CEM_ADC_7],
            adc_mv[CEM_ADC_8]
        );


        /*
         * -----------------------------------------------------
         * Pack ADC1-ADC4 into CAN ID 0x600
         * -----------------------------------------------------
         */

        pack_adc_value(
            can_data_600,
            0,
            adc_mv[CEM_ADC_1]
        );

        pack_adc_value(
            can_data_600,
            2,
            adc_mv[CEM_ADC_2]
        );

        pack_adc_value(
            can_data_600,
            4,
            adc_mv[CEM_ADC_3]
        );

        pack_adc_value(
            can_data_600,
            6,
            adc_mv[CEM_ADC_4]
        );


        /*
         * -----------------------------------------------------
         * Pack ADC5-ADC8 into CAN ID 0x601
         * -----------------------------------------------------
         */

        pack_adc_value(
            can_data_601,
            0,
            adc_mv[CEM_ADC_5]
        );

        pack_adc_value(
            can_data_601,
            2,
            adc_mv[CEM_ADC_6]
        );

        pack_adc_value(
            can_data_601,
            4,
            adc_mv[CEM_ADC_7]
        );

        pack_adc_value(
            can_data_601,
            6,
            adc_mv[CEM_ADC_8]
        );


        /*
         * -----------------------------------------------------
         * Transmit CAN ID 0x600
         * ADC1 - ADC4
         * -----------------------------------------------------
         */

        err = cem_can_transmit(
            0x600,
            can_data_600,
            8
        );

        if (err == ESP_OK)
        {
            ESP_LOGI(
                TAG,
                "CAN 0x600: %02X %02X %02X %02X %02X %02X %02X %02X",
                can_data_600[0],
                can_data_600[1],
                can_data_600[2],
                can_data_600[3],
                can_data_600[4],
                can_data_600[5],
                can_data_600[6],
                can_data_600[7]
            );
        }
        else
        {
            ESP_LOGE(
                TAG,
                "CAN 0x600 transmit failed: %s",
                esp_err_to_name(err)
            );
        }


        /*
         * -----------------------------------------------------
         * Transmit CAN ID 0x601
         * ADC5 - ADC8
         * -----------------------------------------------------
         */

        err = cem_can_transmit(
            0x601,
            can_data_601,
            8
        );

        if (err == ESP_OK)
        {
            ESP_LOGI(
                TAG,
                "CAN 0x601: %02X %02X %02X %02X %02X %02X %02X %02X",
                can_data_601[0],
                can_data_601[1],
                can_data_601[2],
                can_data_601[3],
                can_data_601[4],
                can_data_601[5],
                can_data_601[6],
                can_data_601[7]
            );
        }
        else
        {
            ESP_LOGE(
                TAG,
                "CAN 0x601 transmit failed: %s",
                esp_err_to_name(err)
            );
        }


        /*
         * -----------------------------------------------------
         * Bench-test update rate: 1 Hz
         * -----------------------------------------------------
         */

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}