idf_component_register(
        SRCS 
            "main.c"
            "drivers/cem_adc.c"
            "drivers/cem_can.c"

        INCLUDE_DIRS 
            "." 
            "config"
            "drivers"

        PRIV_REQUIRES
            esp_adc
            esp_driver_gpio
            esp_driver_twai
) 
