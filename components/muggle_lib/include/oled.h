#ifndef __OLED_H__
#define __OLED_H__

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define OLED_SCLK 18
#define OLED_SDIN 23
#define OLED_RST 15
#define OLED_DC 22
#define OLED_CS 5

#define OLED_SMALL_FONT 12
#define OLED_MIDDLE_FONT 16
#define OLED_LARGE_FONT 24

void spi_master_init(spi_host_device_t host_id, int dma_chan, uint32_t max_tran_size, gpio_num_t miso_io_num, gpio_num_t mosi_io_num, gpio_num_t clk_io_num);
void OLED_Init(spi_host_device_t host_id, uint32_t clk_speed);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t size1, char *chr, ...);

#endif
