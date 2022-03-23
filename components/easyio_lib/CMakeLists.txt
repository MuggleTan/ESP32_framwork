set(easyio_srcs
    "src/gpioX.c"
    "src/key.c"
    "src/ledc_pwm.c"
    "src/adc_sampling.c")

idf_component_register(SRCS "${easyio_srcs}"
                       INCLUDE_DIRS "include"
                       "${IDF_PATH}/components/esp_adc_cal/include")
