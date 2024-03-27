#include "reset_task.h"

#define RESET_BUTTON GPIO_NUM_0

ResetTask::ResetTask(const uint8_t task_core) : Task("ResetTask", 2048, task_core)
{
}

ResetTask::~ResetTask()
{
}

void ResetTask::run()
{
    long reset_button_pressed;
    while (1)
    {
        if (gpio_get_level(RESET_BUTTON) == 1)
        {
            reset_button_pressed = millis();
        }
        else
        {
            if (millis() - reset_button_pressed > RESET_SECONDS * 1000)
            {
                esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

                if (it != NULL)
                {
                    const esp_partition_t *factory = esp_partition_get(it);
                    esp_partition_iterator_release(it);
                    if (esp_ota_set_boot_partition(factory) == ESP_OK)
                    {
                        esp_restart();
                    }
                    else
                    {
                    }
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}