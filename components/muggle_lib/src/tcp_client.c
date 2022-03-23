/*
 * @Author: muggle
 * @Date: 2022-02-25 13:12:35
 * @LastEditors: muggle
 * @LastEditTime: 2022-03-11 10:43:15
 * @Description:
 */

#include "tcp_client.h"

static const char *TAG = "example";

extern TaskHandle_t data_upload_task_handler;

int connect_tcp_server(const char *ip, uint16_t port)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    while (1)
    {
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(100);
            continue;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip, port);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            vTaskDelay(100);
            continue;
        }
        ESP_LOGI(TAG, "Successfully connected");

        vTaskResume(data_upload_task_handler);
        
        return sock;
    }
}