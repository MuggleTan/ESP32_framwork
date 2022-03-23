/*
 * @Author: muggle
 * @Date: 2022-02-23 19:24:30
 * @LastEditors: muggle
 * @LastEditTime: 2022-02-26 15:50:11
 * @Description: DHT11驱动
 */
#include "DHT11.h"

void set_DHT11_in(void)
{
    // gpiox_set_input(DHT11_PIN, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE);
    gpio_pad_select_gpio(DHT11_PIN);
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}

void set_DHT11_state(uint8_t state)
{
    gpiox_set_ppOutput(DHT11_PIN, state);
}

uint8_t get_DHT11_state()
{
    return (uint8_t)gpiox_get_level(DHT11_PIN);
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_is_online(void)
{
    uint8_t retry = 0;

    set_DHT11_state(LOW);
    delay_ms(20);
    set_DHT11_state(HIGH);
    delay_us(30);

    set_DHT11_in();

    while (!get_DHT11_state() && retry++ < 100) // DHT11会拉低80us
        delay_us(1);

    if (retry >= 100)
        return 1;

    retry = 0;

    while (get_DHT11_state() && retry++ < 100) // DHT11拉低后会再次拉高80us
        delay_us(1);

    if (retry >= 100)
        return 1;

    return 0;
}

uint8_t DHT11_read_bit(void)
{
    uint8_t retry = 0;

    while ((get_DHT11_state() == 1) && retry++ < 100)
        delay_us(1);

    retry = 0;

    while ((get_DHT11_state() == 0) && retry++ < 100)
        delay_us(1);

    delay_us(40); //等待40us

    return get_DHT11_state();
}

uint8_t DHT11_read_byte(void)
{
    uint8_t i, dat = 0;

    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_read_bit();
    }

    return dat;
}

//从DHT11读取一次数据
// temp:温度值(范围:0~50°)
// humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t DHT11_read_data(float *temp, uint8_t *hum)
{
    uint8_t buf[5];
    uint8_t i;

    if (DHT11_is_online() == 0)
    {
        for (i = 0; i < 5; i++) //读取40位数据
        {
            buf[i] = DHT11_read_byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *hum = buf[0];
            *temp = buf[2] + buf[3] / 10.0;
        }
        else
        {
            printf("%d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
            printf("DHT11 check sum error\n");
            return 1;
        }
    }
    else
    {
        printf("DHT11 is not online\n");
        return 1;
    }

    return 0;
}
