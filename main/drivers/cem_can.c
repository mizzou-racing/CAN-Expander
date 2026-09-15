#include "cem_can.h"
#include "cem_pins.h"

#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_log.h"

static const char *TAG = "CEM_CAN";

static twai_node_handle_t can_node = NULL;

esp_err_t cem_can_init(void)
{
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = CEM_CAN_TX_GPIO,
        .io_cfg.rx = CEM_CAN_RX_GPIO,

        .bit_timing.bitrate = 1000000,

        .tx_queue_depth = 5,

        .fail_retry_cnt = -1,
    };

    esp_err_t err = twai_new_node_onchip(
        &node_config,
        &can_node
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "CAN node creation failed: %s",
            esp_err_to_name(err)
        );

     return err;
    }

    err = twai_node_enable(can_node);

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "CAN node enable failed: %s",
            esp_err_to_name(err)
        );

     return err;
    }

    ESP_LOGI(TAG, "CAN initialized at 1 Mbit/s");

    return ESP_OK;
}

esp_err_t cem_can_transmit(
    uint32_t id,
    const uint8_t *data,
    uint8_t length
)
{
    if (data == NULL || length > 8)
    {
        return ESP_ERR_INVALID_ARG;
    }

    twai_frame_t frame = {
        .header.id = id,
        .buffer = (uint8_t *)data,
        .buffer_len = length,
    };

    return twai_node_transmit(
        can_node,
        &frame,
        100
    );
}
