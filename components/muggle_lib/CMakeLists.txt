set(muggle_srcs
    "src/delay.c"
    "src/DHT11.c"
    "src/oled.c"
    "src/tcp_client.c")

idf_component_register(SRCS "${muggle_srcs}"
                       PRIV_REQUIRES nvs_flash protocol_examples_common
                       INCLUDE_DIRS "include"
                       "../easyio_lib/include")
