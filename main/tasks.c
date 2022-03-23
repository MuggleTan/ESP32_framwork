/*
 * @Author: muggle
 * @Date: 2022-03-06 10:37:09
 * @LastEditors: muggle
 * @LastEditTime: 2022-03-11 13:58:58
 * @Description:
 */

#include "easyio.h"
#include "DHT11.h"
#include "tcp_client.h"
#include "oled.h"
#include "tasks.h"

#define BLINK_GPIO 2
#define FAN_GPIO 21
#define HEATING_GPIO 18
#define HUMIDIFY_GPIO 19
#define BUZZER_GPIO 26
#define LED_GPIO 4

#define BOOT 0

#define HOST_IP "192.168.137.1"
#define HOST_PORT 5000

EvnData evnData;
DeviceState deviceState;

uint8_t OLED_page = 0;
uint8_t MODE = 0;

int sock = -1;

char pwm_queue[2];

TaskHandle_t led_task_handler;
TaskHandle_t warn_task_handler;
TaskHandle_t oled_task_handler;
TaskHandle_t data_upload_task_handler;

void led_task(void *args)
{
    // gpiox_set_ppOutput(BLINK_GPIO, 1);

    while (1)
    {
        // gpiox_set_level(BLINK_GPIO, 1);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // gpiox_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DHT11_task(void *args)
{

    while (1)
    {
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        if (DHT11_read_data(&evnData.temp, &evnData.hum) == 0)
        {
            printf("temp: %.1f℃  hum: %d%% RH\n", evnData.temp, evnData.hum);
        }
    }
}

void adc1_scan_task(void *args)
{
    adc1_init_with_calibrate(ADC_ATTEN_DB_11, 2, ADC_CHANNEL_6, ADC_CHANNEL_7);
    // 配置ADC_CON为推挽输出，设置初始电平为1，开通光敏电路和电池分压采样电路的供电（ADC闲时，ADC_CON可设置输出为0，关断电压输出，以降低分压电阻上功耗）
    gpiox_set_ppOutput(12, 1);

    while (1)
    {
        // evnData.soil = adc1_cal_get_voltage_mul(ADC_CHANNEL_6, 16) / 33.0;
        // evnData.light = adc1_cal_get_voltage_mul(ADC_CHANNEL_7, 16) / 33.0;

        // printf("soil:%.2f%% light:%.2f%%\t\n", evnData.soil, evnData.light);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = "192.168.137.1";

    while (1)
    {
        sock = connect_tcp_server(host_ip, HOST_PORT);

        while (1)
        {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0)
            {
                ESP_LOGE("TCP_CLIENT_TASK", "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else
            {
                rx_buffer[len] = 0; // Null - terminate whatever we received and treat like a string
                ESP_LOGI("TCP_CLIENT_TASK", "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI("TCP_CLIENT_TASK", "%s", rx_buffer);

                if (strcmp(rx_buffer, "open_led") == 0)
                {
                    gpiox_set_level(LED_GPIO, 0);
                }
                else if (strcmp(rx_buffer, "close_led") == 0)
                {
                    gpiox_set_level(LED_GPIO, 1);
                }
                // else if (strcmp(rx_buffer, "open_pump") == 0)
                // {
                //     gpiox_set_level(PUMP_GPIO, 1);
                // }
                // else if (strcmp(rx_buffer, "close_pump") == 0)
                // {
                //     gpiox_set_level(PUMP_GPIO, 0);
                // }
                else if (strcmp(rx_buffer, "open_fan") == 0)
                {
                    gpiox_set_level(FAN_GPIO, 1);
                }
                else if (strcmp(rx_buffer, "close_fan") == 0)
                {
                    gpiox_set_level(FAN_GPIO, 0);
                }
                else if (strcmp(rx_buffer, "open_humidify") == 0)
                {
                    gpiox_set_level(HUMIDIFY_GPIO, 1);
                }
                else if (strcmp(rx_buffer, "close_humidify") == 0)
                {
                    gpiox_set_level(HUMIDIFY_GPIO, 0);
                }

                if (send(sock, "OK\n", strlen("OK\n"), 0) < 0)
                {
                    ESP_LOGE("TCP_CLIENT_TASK", "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1)
        {
            ESP_LOGE("TCP_CLIENT_TASK", "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}

void oled_task(void *args)
{
    OLED_Init(SPI3_HOST, 1000 * 1000 * 40);
    OLED_ShowString(0, 52, OLED_SMALL_FONT, "AUTO");

    while (1)
    {
        if (OLED_page == 0)
        {
            // OLED_ShowChinese(20, 5, 1, 16);
            // OLED_ShowChinese(38, 5, 2, 16);

            // OLED_ShowChinese(70, 5, 0, 16);
            // OLED_ShowChinese(88, 5, 2, 16);

            // OLED_ShowString(18, 30, OLED_MIDDLE_FONT, "%d%%RH", evnData.hum);
            // OLED_ShowString(66, 30, OLED_MIDDLE_FONT, "%.1f", evnData.temp);
            // OLED_ShowChinese(100, 31, 3, 16);

            // OLED_Refresh();
        }
        else if (OLED_page == 1)
        {
            // OLED_ShowChinese(20, 5, 4, 16);
            // OLED_ShowChinese(38, 5, 5, 16);

            // OLED_ShowChinese(70, 5, 6, 16);
            // OLED_ShowChinese(88, 5, 7, 16);

            // OLED_ShowString(20, 30, OLED_MIDDLE_FONT, "%.1f%%", evnData.soil);
            // OLED_ShowString(68, 30, OLED_MIDDLE_FONT, "%.1f%%", evnData.light);

            // OLED_Refresh();
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void device_monitor_task(void *args)
{
    gpiox_set_ppOutput(FAN_GPIO, 0);
    // gpiox_set_ppOutput(PUMP_GPIO, 0);
    gpiox_set_ppOutput(HUMIDIFY_GPIO, 0);
    gpiox_set_ppOutput(LED_GPIO, 0);

    while (1)
    {
        if (MODE == 0)
        {
            // if (evnData.hum <= deviceState.hum_limit)
            // {
            //     deviceState.humidify = 1;
            //     gpiox_set_level(HUMIDIFY_GPIO, 1);
            // }
            // else if (deviceState.warn_flag == 1)
            // {
            //     deviceState.humidify = 0;
            //     gpiox_set_level(HUMIDIFY_GPIO, 0);
            // }

            // if (evnData.temp >= deviceState.temp_limit)
            // {
            //     deviceState.fan = 1;
            //     gpiox_set_level(FAN_GPIO, 1);
            // }
            // else if (deviceState.warn_flag == 1)
            // {
            //     deviceState.fan = 0;
            //     gpiox_set_level(FAN_GPIO, 0);
            // }

            // if (evnData.light <= deviceState.light_limit)
            // {
            //     deviceState.led = 1;
            //     gpiox_set_level(LED_GPIO, 0);
            // }
            // else
            // {
            //     deviceState.led = 0;
            //     gpiox_set_level(LED_GPIO, 1);
            // }

            // if (evnData.soil <= deviceState.soil_limit)
            // {
            //     deviceState.pump = 1;
            //     gpiox_set_level(PUMP_GPIO, 1);
            // }
            // else if (deviceState.warn_flag == 1)
            // {
            //     deviceState.pump = 0;
            //     gpiox_set_level(PUMP_GPIO, 0);
            // }
        }

        // if (evnData.hum <= deviceState.hum_limit || evnData.temp >= deviceState.temp_limit ||
        //     evnData.light <= deviceState.light_limit || evnData.soil <= deviceState.soil_limit)
        // {
        //     deviceState.warn_flag = 1;
        //     vTaskResume(warn_task_handler);
        // }
        // else
        // {
        //     deviceState.warn_flag = 0;
        //     vTaskSuspend(warn_task_handler);
        // }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// key_scan_task 任务，单独运行一个任务，运行按键检测（滤除按键抖动、短按、长按）
void key_scan_task(void *arg)
{
    // 按键检测任务
    key_scan(1, BOOT); // 2个按键
}

// key_catch_task 任务。去捕获按键事件，并控制LED任务状态。
void key_catch_task(void *arg)
{
    uint32_t key_event = 0;
    uint32_t key_gpio = 0; // 触发按键事件的按键gpio序号
    uint32_t key_type = 0; // 触发按键事件的类型

    while (1)
    {
        // 以阻塞方式，不断读取 key_evt_queue 队列信息，监控按键事件
        if (xQueueReceive(key_evt_queue, &key_event, portMAX_DELAY))
        {
            // 拆分按键事件，得到按键值和按键类型
            key_gpio = key_event & 0x0000FFFF; // 按键值
            key_type = key_event >> 16;        // 按键类型（1为短按，2为长按）

            ESP_LOGW("KEY_TASK", "key:%d, type:%d\n", key_gpio, key_type); // 输出按键事件

            if (key_gpio == BOOT)
            {
                if (key_type == 1)
                {
                    // if (deviceState.fan)
                    // {
                    //     gpiox_set_level(FAN_GPIO, 0);
                    //     deviceState.fan = 0;
                    // }
                    // else if (deviceState.fan == 0)
                    // {
                    //     gpiox_set_level(FAN_GPIO, 1);
                    //     deviceState.fan = 1;
                    // }
                }
                else if (key_type == 2)
                {
                    // vTaskSuspend(oled_task_handler);
                    // if (OLED_page == 0)
                    // {
                    //     OLED_Clear();

                    //     OLED_ShowChinese(20, 5, 4, 16);
                    //     OLED_ShowChinese(38, 5, 5, 16);

                    //     OLED_ShowChinese(70, 5, 6, 16);
                    //     OLED_ShowChinese(88, 5, 7, 16);

                    //     OLED_ShowString(20, 30, OLED_MIDDLE_FONT, "%.1f%% ", evnData.soil);
                    //     OLED_ShowString(68, 30, OLED_MIDDLE_FONT, "%.1f%% ", evnData.light);

                    //     OLED_Refresh();
                    //     OLED_page = 1;
                    // }
                    // else if (OLED_page == 1)
                    // {
                    //     OLED_Clear();

                    //     OLED_ShowChinese(20, 5, 1, 16);
                    //     OLED_ShowChinese(38, 5, 2, 16);

                    //     OLED_ShowChinese(70, 5, 0, 16);
                    //     OLED_ShowChinese(88, 5, 2, 16);

                    //     OLED_ShowString(18, 30, OLED_MIDDLE_FONT, "%d%%RH", evnData.hum);
                    //     OLED_ShowString(66, 30, OLED_MIDDLE_FONT, "%.1f", evnData.temp);
                    //     OLED_ShowChinese(100, 31, 3, 16);

                    //     OLED_Refresh();
                    //     OLED_page = 0;
                    // }
                    // if (MODE == 0)
                    // {
                    //     OLED_ShowString(0, 52, OLED_SMALL_FONT, "MANUAL");
                    //     OLED_Refresh();
                    // }
                    // else if (MODE == 1)
                    // {
                    //     OLED_ShowString(0, 52, OLED_SMALL_FONT, "AUTO   ");
                    //     OLED_Refresh();
                    // }
                    // vTaskResume(oled_task_handler);
                }
            }
        }
    }
}

void warn_task(void *args)
{
    gpiox_set_ppOutput(BUZZER_GPIO, 0);

    while (1)
    {
        // if (deviceState.silence_flag)
        // {
        //     gpiox_set_level(BUZZER_GPIO, 0);
        //     vTaskDelay(5000 / portTICK_PERIOD_MS);
        // }
        // else
        {
            gpiox_set_level(BUZZER_GPIO, 0);
            vTaskDelay(400 / portTICK_PERIOD_MS);
            gpiox_set_level(BUZZER_GPIO, 1);
            vTaskDelay(400 / portTICK_PERIOD_MS);
        }
    }
}

void data_upload_task(void *args)
{
    char buf[128];
    while (1)
    {
        // sprintf(buf, "#,%.1f,%d,%.2f,%.2f,$", evnData.temp, evnData.hum, evnData.soil, evnData.light);
        // send(sock, buf, strlen(buf), 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void pwm_task(void* arg)
{
    int ch=0;
    //初始化高速ledc通道，5KHz，13Bit分辨率，共使用3个通道，对应RGB-LED的管脚
    ledc_pwm_hs_init();
    while (1) {
        //3s内：高速LEDc，0~2通道，0->50%占空比渐变（满分辨率为8191）
        printf("(1) LEDC HS_RGB fade up to duty = %d%%\n", 50);
        for (ch = 0; ch < LEDC_HS_CH_NUM; ch++) {
            ledc_pwm_set_fade_duty_cycle(ledc_hs_ch, ch, 1000, LEDC_TEST_FADE_TIME);
        }
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);
        
        //3s内：高速LEDc，0~2通道，50%->0渐变
        printf("(2) LEDC HS_RGB fade down to duty = 0%%\n");
        for (ch = 0; ch < LEDC_HS_CH_NUM; ch++) {
            ledc_pwm_set_fade_duty_cycle(ledc_hs_ch, ch, 0, LEDC_TEST_FADE_TIME);
        }
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);
    }
}