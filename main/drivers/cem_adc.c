#include "cem_adc.h"
#include "cem_pins.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "esp_log.h"

static const char *TAG = "CEM_ADC";

/*ESP32-S3 ADC hardware handles:*/
static adc_oneshot_unit_handle_t adc_unit1_handle;
static adc_oneshot_unit_handle_t adc_unit2_handle;

/*ADC calibration handles*/
static adc_cali_handle_t adc_unit1_cal_handle;
static adc_cali_handle_t adc_unit2_cal_handle;

typedef struct 
{
    adc_unit_t unit;
    adc_channel_t channel;

}

cem_adc_config_t;

static const cem_adc_config_t adc_config[CEM_ADC_COUNT] =
{
    [CEM_ADC_1] = {
        .unit = CEM_ADC1_UNIT,
        .channel = CEM_ADC1_CHANNEL
    },

    [CEM_ADC_2] = {
    .unit = CEM_ADC2_UNIT,
    .channel = CEM_ADC2_CHANNEL
    },

    [CEM_ADC_3] = {
    .unit = CEM_ADC3_UNIT,
    .channel = CEM_ADC3_CHANNEL
    },

    [CEM_ADC_4] = {
        .unit = CEM_ADC4_UNIT,
        .channel = CEM_ADC4_CHANNEL
    },

    [CEM_ADC_5] = {
        .unit = CEM_ADC5_UNIT,
        .channel = CEM_ADC5_CHANNEL
    },

    [CEM_ADC_6] = {
        .unit = CEM_ADC6_UNIT,
        .channel = CEM_ADC6_CHANNEL
    },

    [CEM_ADC_7] = {
        .unit = CEM_ADC7_UNIT,
        .channel = CEM_ADC7_CHANNEL
    },

    [CEM_ADC_8] = {
        .unit = CEM_ADC8_UNIT,
        .channel = CEM_ADC8_CHANNEL
    }
};

esp_err_t cem_adc_init(void)
{
    esp_err_t err;

    adc_oneshot_unit_init_cfg_t unit1_config = 
    {.unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    adc_oneshot_unit_init_cfg_t unit2_config = 
    {.unit_id = ADC_UNIT_2,
    .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    err = adc_oneshot_new_unit(
        &unit1_config,
        &adc_unit1_handle
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initalize ADC Unit 1");
        return err;
    }

     err = adc_oneshot_new_unit(
        &unit2_config,
        &adc_unit2_handle
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initalize ADC Unit 2");
        return err;
    }

    adc_cali_curve_fitting_config_t unit1_cali_config = 
    {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };

    adc_cali_curve_fitting_config_t unit2_cali_config = 
    {
        .unit_id = ADC_UNIT_2,
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };

    err = adc_cali_create_scheme_curve_fitting(
        &unit1_cali_config,
        &adc_unit1_cal_handle
    );

    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC Unit 1 Calibration failed");
        return err;
    }
    err = adc_cali_create_scheme_curve_fitting(
        &unit2_cali_config,
        &adc_unit2_cal_handle
    );

    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC Unit 2 Calibration failed");
        return err;
    }
    //return ESP_OK;

    adc_oneshot_chan_cfg_t channel_config = 
    {
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };

    for (int i =0; i < CEM_ADC_COUNT; i++)
    {
        adc_oneshot_unit_handle_t unit_handle;

        if (adc_config[i].unit == ADC_UNIT_1)
        {
            unit_handle = adc_unit1_handle;
        }
        else
        {
            unit_handle = adc_unit2_handle;
        }

        err = adc_oneshot_config_channel(
            unit_handle,
            adc_config[i].channel,
            &channel_config
        );

        if (err != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Failed to configure CEM ADC%d",i + 1
            );

            return err;
        }
    }
        ESP_LOGI(TAG, "All CEM ADC channels initialized");

        return ESP_OK;
    }

    esp_err_t cem_adc_read(
        cem_adc_channel_t channel,
        int *millivolts
    )
    {
        if (channel >= CEM_ADC_COUNT || millivolts == NULL)
        {
            return ESP_ERR_INVALID_ARG;
        }

        adc_oneshot_unit_handle_t unit_handle;
        adc_cali_handle_t cal_handle;

        if (adc_config[channel].unit == ADC_UNIT_1)
        {
            unit_handle = adc_unit1_handle;
            cal_handle = adc_unit1_cal_handle;
        }
        else
        {
            unit_handle = adc_unit2_handle;
            cal_handle = adc_unit2_cal_handle;
        }

        int raw = 0;

        esp_err_t err = adc_oneshot_read(
            unit_handle,
            adc_config[channel].channel,
            &raw
        );

        if (err != ESP_OK)
        {
            return err;
        }

        err = adc_cali_raw_to_voltage(
            cal_handle,
            raw,
            millivolts
        );

        return err;
    }

