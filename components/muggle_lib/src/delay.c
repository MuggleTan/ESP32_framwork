/*
 * @Author: muggle
 * @Date: 2022-02-23 22:12:17
 * @LastEditors: muggle
 * @LastEditTime: 2022-02-25 15:36:48
 * @Description: 
 */
#include "delay.h"

void delay_ms(uint16_t ms)
{
    int i = 0;
    for (i = 0; i < ms; i++)
    {
        ets_delay_us(1000);
    }
}
