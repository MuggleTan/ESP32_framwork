/*
 * @Author: muggle
 * @Date: 2022-03-06 10:37:22
 * @LastEditors: muggle
 * @LastEditTime: 2022-03-11 10:38:23
 * @Description:
 */
#ifndef __TASKS_H__
#define __TASKS_H__

void DHT11_task(void *args);
void adc1_scan_task(void *args);
void tcp_client_task(void *args);
void led_task(void *args);
void oled_task(void *args);
void device_monitor_task(void *args);
void key_scan_task(void *args);
void key_catch_task(void *args);
void warn_task(void *args);
void data_upload_task(void *args);
void pwm_task(void *args);

typedef struct EvnData
{
    float temp;
    uint8_t hum;

} EvnData;

typedef struct DeviceState
{

} DeviceState;

extern EvnData evnData;
extern DeviceState deviceState;
extern char pwm_queue[2];

extern TaskHandle_t led_task_handler;
extern TaskHandle_t warn_task_handler;
extern TaskHandle_t oled_task_handler;
extern TaskHandle_t data_upload_task_handler;

#endif
