/*
 * @Author: muggle
 * @Date: 2022-02-23 22:12:22
 * @LastEditors: muggle
 * @LastEditTime: 2022-02-25 15:39:24
 * @Description: 
 */
#ifndef __DELAY_H__
#define __DELAY_H__

#include "esp_event.h"

#define delay_us(x) ets_delay_us(x)
void delay_ms(uint16_t ms);


#endif
