#define CEM_CAN_TX_GPIO GPIO_NUM_45
#define CEM_CAN_RX_GPIO GPIO_NUM_48

#pragma once

#include <stdint.h>
#include "esp_err.h"

esp_err_t cem_can_init(void);

esp_err_t cem_can_transmit(
    uint32_t id,
    const uint8_t *data,
    uint8_t length
);
