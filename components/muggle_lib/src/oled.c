/*
 * @Author: muggle
 * @Date: 2022-02-26 14:36:12
 * @LastEditors: muggle
 * @LastEditTime: 2022-03-06 16:15:17
 * @Description: SPI驱动
 */

#include <string.h>
#include "oled.h"
#include "oled_font.h"
#include "delay.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define OLED_CMD 0
#define OLED_DATA 1

spi_device_handle_t OLED_SPI = NULL;

uint8_t OLED_GRAM[128][8] = {0};

/**
 * @brief  使用spi发送数据或命令
 *      - 使用spi_device_polling_transmit，它等待直到传输完成。
 *      - 发送时同时设置D/C线为0，传输命令
 *      - spi_send_byte(LCD_SPI, 0x04, OLED_CMD);
 *
 * @param  spi LCD与SPI关联的句柄，通过此来调用SPI总线上的LCD设备
 * @param  data 向LCD发送的1个字节命令内容
 * @param  type 0-发送命令 1-发送数据
 *
 * @return
 *     - none
 */
void spi_send_byte(const uint8_t data, const uint8_t type)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));                        // 清空传输结构体
    t.length = 8;                                    // SPI传输lcd命令的长度：8Bit。1个字节。
    t.tx_buffer = &data;                             // 数据是cmd本身
    t.user = (void *)type;                           // D/C 线电平为0，传输命令
    ret = spi_device_polling_transmit(OLED_SPI, &t); // 开始传输
    assert(ret == ESP_OK);                           // 应该没有问题
}

//开启OLED显示
void OLED_DisPlay_On(void)
{
    spi_send_byte(0x8D, OLED_CMD); //电荷泵使能
    spi_send_byte(0x14, OLED_CMD); //开启电荷泵
    spi_send_byte(0xAF, OLED_CMD); //点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void)
{
    spi_send_byte(0x8D, OLED_CMD); //电荷泵使能
    spi_send_byte(0x10, OLED_CMD); //关闭电荷泵
    spi_send_byte(0xAF, OLED_CMD); //关闭屏幕
}

//画点
// x:0~127
// y:0~63
void OLED_DrawPoint(uint8_t x, uint8_t y)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_GRAM[x][i] |= n;
}

//清除一个点
// x:0~127
// y:0~63
void OLED_ClearPoint(uint8_t x, uint8_t y)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    OLED_GRAM[x][i] |= n;
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// size:选择字体 12/16/24
//取模方式 逐列式
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1)
{
    uint8_t i, m, temp, size2, chr1;
    uint8_t y0 = y;
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); //得到字体一个字符对应点阵集所占的字节数
    chr1 = chr - ' ';                                          //计算偏移后的值
    for (i = 0; i < size2; i++)
    {
        if (size1 == 12)
        {
            temp = asc2_1206[chr1][i];
        } //调用1206字体
        else if (size1 == 16)
        {
            temp = asc2_1608[chr1][i];
        } //调用1608字体
        else if (size1 == 24)
        {
            temp = asc2_2412[chr1][i];
        } //调用2412字体
        else
            return;
        for (m = 0; m < 8; m++) //写入数据
        {
            if (temp & 0x80)
                OLED_DrawPoint(x, y);
            else
                OLED_ClearPoint(x, y);
            temp <<= 1;
            y++;
            if ((y - y0) == size1)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//显示字符串
// x,y:起点坐标
// size1:字体大小
//*chr:字符串起始地址
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t size1, char *format, ...)
{
    va_list ap;
    uint8_t buf[32], i = 0;
    va_start(ap, format);
    vsprintf((char *)buf, format, ap);
    va_end(ap);
    

    while ((buf[i] >= ' ') && (buf[i] <= '~')) //判断是不是非法字符!
    {
        OLED_ShowChar(x, y, buf[i], size1);
        x += size1 / 2;
        if (x > 128 - size1) //换行
        {
            x = 0;
            y += 2;
        }
        i++;
    }

}

//更新显存到OLED
void OLED_Refresh(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        spi_send_byte(0xb0 + i, OLED_CMD); //设置行起始地址
        spi_send_byte(0x00, OLED_CMD);     //设置低列起始地址
        spi_send_byte(0x10, OLED_CMD);     //设置高列起始地址
        for (n = 0; n < 128; n++)
            spi_send_byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

//清屏函数
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0; //清除所有数据
        }
    }
    OLED_Refresh(); //更新显示
}

//显示汉字
// x,y:起点坐标
// num:汉字对应的序号
//取模方式 列行式
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1)
{
    uint8_t i, m, n = 0, temp, chr1;
    uint8_t x0 = x, y0 = y;
    uint8_t size3 = size1 / 8;

    while (size3--)
    {
        chr1 = num * size1 / 8 + n;
        n++;
        for (i = 0; i < size1; i++)
        {
            if (size1 == 16)
            {
                temp = chinese[chr1][i];
            } //调用16*16字体
            // else if (size1 == 24)
            // {
            //     temp = Hzk2[chr1][i];
            // } //调用24*24字体
            // else if (size1 == 32)
            // {
            //     temp = Hzk3[chr1][i];
            // } //调用32*32字体
            // else if (size1 == 64)
            // {
            //     temp = Hzk4[chr1][i];
            // } //调用64*64字体
            else
                return;

            for (m = 0; m < 8; m++)
            {
                if (temp & 0x01)
                    OLED_DrawPoint(x, y);
                else
                    OLED_ClearPoint(x, y);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == size1)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

/**
 * @brief  配置SPIx主机模式，配置DMA通道、DMA字节大小，及 MISO、MOSI、CLK的引脚。
 *      - （注意：普通GPIO最大只能30MHz，而IOMUX默认的SPI-IO，CLK最大可以设置到80MHz）
 *      - 例：spi_master_init(SPI2_HOST, LCD_DEF_DMA_CHAN, LCD_DMA_MAX_SIZE, SPI2_DEF_PIN_NUM_MISO, SPI2_DEF_PIN_NUM_MOSI, SPI2_DEF_PIN_NUM_CLK);
 *
 * @param  host_id SPI端口号。SPI1_HOST / SPI2_HOST / SPI3_HOST
 * @param  dma_chan 使用的DMA通道
 * @param  max_tran_size DMA最大的传输字节数（会根据此值给DMA分配内存，值越大分配给DMA的内存就越大，单次可用DMA传输的内容就越多）
 * @param  miso_io_num MISO端口号。除仅能做输入 和 6、7、8、9、10、11之外的任意端口，但仅IOMUX默认的SPI-IO才能达到最高80MHz上限。
 * @param  mosi_io_num MOSI端口号
 * @param  clk_io_num CLK端口号
 *
 * @return
 *     - none
 */
void spi_master_init(spi_host_device_t host_id, int dma_chan, uint32_t max_tran_size, gpio_num_t miso_io_num, gpio_num_t mosi_io_num, gpio_num_t clk_io_num)
{
    esp_err_t ret;
    // 配置 MISO、MOSI、CLK、CS 的引脚，和DMA最大传输字节数
    spi_bus_config_t buscfg = {
        .miso_io_num = miso_io_num, // MISO引脚定义
        .mosi_io_num = mosi_io_num, // MOSI引脚定义
        .sclk_io_num = clk_io_num,  // CLK引脚定义
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = max_tran_size, // 最大传输字节数
    };

    // 初始化SPI总线
    ret = spi_bus_initialize(host_id, &buscfg, dma_chan);
    ESP_ERROR_CHECK(ret);
}

// SPI预传输回调。（在SPI传输即将开始前调用的函数）用来处理 D/C 线的电平，来控制接下来发送的内容是 指令/数据
static void spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(OLED_DC, dc);
}

void OLED_Init(spi_host_device_t host_id, uint32_t clk_speed)
{
    esp_err_t ret;

    spi_master_init(SPI3_HOST, 2, 128 * 8, -1, OLED_SDIN, OLED_SCLK);

    // LCD设备初始化
    // 先关联 SPI总线及LCD设备
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clk_speed,         // CLK时钟频率
        .mode = 0,                           // SPI mode 0
        .spics_io_num = OLED_CS,             // CS引脚定义
        .queue_size = 7,                     // 事务队列大小为7
        .pre_cb = spi_pre_transfer_callback, // 指定预传输回调，以处理 D/C线电平，来区别发送命令/数据
    };
    // 将LCD外设与SPI总线关联
    ret = spi_bus_add_device(host_id, &devcfg, &OLED_SPI);
    ESP_ERROR_CHECK(ret);

    // 初始化 DC、RST、BLK 引脚，配置为推挽输出
    gpio_set_direction(OLED_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(OLED_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(OLED_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(OLED_SCLK, GPIO_MODE_OUTPUT);

    // RST引脚拉低，复位后重新拉高
    gpio_set_level(OLED_RST, 0);
    delay_ms(100);
    gpio_set_level(OLED_RST, 1);
    delay_ms(200);

    spi_send_byte(0xAE, OLED_CMD); //--turn off oled panel
    spi_send_byte(0x00, OLED_CMD); //---set low column address
    spi_send_byte(0x10, OLED_CMD); //---set high column address
    spi_send_byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    spi_send_byte(0x81, OLED_CMD); //--set contrast control register
    spi_send_byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
    spi_send_byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    spi_send_byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    spi_send_byte(0xA6, OLED_CMD); //--set normal display
    spi_send_byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    spi_send_byte(0x3f, OLED_CMD); //--1/64 duty
    spi_send_byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    spi_send_byte(0x00, OLED_CMD); //-not offset
    spi_send_byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
    spi_send_byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
    spi_send_byte(0xD9, OLED_CMD); //--set pre-charge period
    spi_send_byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    spi_send_byte(0xDA, OLED_CMD); //--set com pins hardware configuration
    spi_send_byte(0x12, OLED_CMD);
    spi_send_byte(0xDB, OLED_CMD); //--set vcomh
    spi_send_byte(0x40, OLED_CMD); // Set VCOM Deselect Level
    spi_send_byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
    spi_send_byte(0x02, OLED_CMD); //
    spi_send_byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
    spi_send_byte(0x14, OLED_CMD); //--set(0x10) disable
    spi_send_byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
    spi_send_byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
    spi_send_byte(0xAF, OLED_CMD);

    OLED_Clear();
}
