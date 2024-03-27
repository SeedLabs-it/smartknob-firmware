#include "reset_task.h"

ResetTask::ResetTask(const uint8_t task_core) : Task("ResetTask", 2048, task_core)
{
}

ResetTask::~ResetTask()
{
}

void ResetTask::run()
{
    // esp_restart();
    long button_pressed;
    while (1)
    {
        // ESP_LOGD("ResetTask", "ResetTask!!!!");
        if (gpio_get_level(GPIO_NUM_0) == 1)
        {
            button_pressed = millis();
        }
        else
        {
            if (millis() - button_pressed > RESET_SECONDS * 1000)
            {
                ESP_LOGD("ResetTask", "Resetting!!!!");
                esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

                if (it != NULL)
                {
                    const esp_partition_t *factory = esp_partition_get(it);
                    esp_partition_iterator_release(it);
                    if (esp_ota_set_boot_partition(factory) == ESP_OK)
                    {
                        ESP_LOGE("ResetTask", "Set boot partition");
                        esp_restart();
                    }
                    else
                    {
                        ESP_LOGE("ResetTask", "Failed to set boot partition");
                    }
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}