/*
 * @Author: muggle
 * @Date: 2022-02-23 19:24:17
 * @LastEditors: muggle
 * @LastEditTime: 2022-03-06 14:18:39
 * @Description:
 */

#ifndef __DHT11_H__
#define __DHT11_H__

#include "gpioX.h"
#include "delay.h"

#define DHT11_PIN 23

#define DHT11_DEBUG 0

#define HIGH 1
#define LOW 0


uint8_t DHT11_read_data(float *temp, uint8_t *hum);

#endif
