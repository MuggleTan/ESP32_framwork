#include "easyio.h"
#include "tasks.h"
#include "blufi_example.h"

#define TAG "MAIN_APP"

void Struct_Init()
{

}

void app_main(void)
{
    Struct_Init();

    // 创建任务，任务栈空间大小为 2048，任务优先级为3（configMINIMAL_STACK_SIZE 不能用 printf，会导致ESP32反复重启）
    xTaskCreate(led_task, "led_task", 1024, NULL, 1, &led_task_handler);
    // xTaskCreate(DHT11_task, "DHT11_task", 4096, NULL, 3, NULL);
    xTaskCreate(pwm_task, "pwm_task", 4096, NULL, 3, NULL);
    // xTaskCreate(warn_task, "warn_task", 2048, NULL, 2, &warn_task_handler);
    // xTaskCreate(adc1_scan_task, "adc1_scan_task", 2048, NULL, 3, NULL);
    // xTaskCreate(oled_task, "oled_task", 2048, NULL, 3, &oled_task_handler);
    // xTaskCreate(key_scan_task, "key_scan_task", 4096, NULL, 3, NULL);
    // xTaskCreate(key_catch_task, "key_catch_task", 4096, NULL, 2, NULL);
    // xTaskCreate(key_catch_task, "key_catch_task", 4096, NULL, 2, NULL);
    // xTaskCreate(data_upload_task, "data_upload_task", 4096, NULL, 3, &data_upload_task_handler);
    // xTaskCreate(device_monitor_task, "device_monitor_task", 1024 * 4, NULL, 3, NULL);

    // vTaskSuspend(warn_task_handler);
    // vTaskSuspend(data_upload_task_handler);

    // xQueueCreate(3, sizeof(pwm_queue));

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    // blufi_run();

    // xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}
