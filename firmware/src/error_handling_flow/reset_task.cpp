#include "reset_task.h"

#define RESET_BUTTON GPIO_NUM_0

ResetTask::ResetTask(const uint8_t task_core, Configuration &configuration) : Task("ResetTask", 1024 * 8, 0, task_core), configuration_(configuration)
{
}

ResetTask::~ResetTask()
{
}

void ResetTask::run()
{
    long reset_button_pressed;
    long reset_button_released;
    bool held = false;
    // uint8_t pressedCount = 0;

    while (1)
    {
        // Button is not pressed.
        if (gpio_get_level(RESET_BUTTON) == 1)
        {
            if (held)
            {
                // pressedCount += 1;
                held = false;

                if (millis() - reset_button_pressed > SOFT_RESET_SECONDS * 1000 && millis() - reset_button_pressed < HARD_RESET_SECONDS * 1000)
                {
                    LOGI("Resetting to factory defaults");
                    softReset();
                }

                WiFiEvent event = {
                    .type = EventType::SK_RESET_BUTTON_RELEASED,
                };
                publishEvent(event);
            }
            reset_button_pressed = millis();
        }
        else // Button is pressed when low (0)
        {

            reset_button_released = millis();

            if (millis() - reset_button_pressed > 1 * 1000)
            {
                if (!held)
                {
                    WiFiEvent event = {
                        .type = EventType::SK_RESET_BUTTON_PRESSED,
                    };
                    publishEvent(event);
                    held = true;
                }
                motor_task_->playHaptic(true, false);
                delay(100);
            }

            if (millis() - reset_button_pressed > HARD_RESET_SECONDS * 1000)
            {
                LOGI("Hard resetting");
                hardReset();
            }
        }

        // if (pressedCount >= 6)
        // {
        // }

        // if (millis() - reset_button_released > 500) // Reset the pressed count after 500ms of no button press
        // {
        //     pressedCount = 0;
        // }

        delay(100);
    }
}

void ResetTask::softReset()
{
    if (configuration_.resetToDefaults())
    {
        LOGI("Resetting to factory defaults");
        esp_restart();
    }
    else
    {
        LOGE("Failed to reset configuration to defaults");
    }
}

void ResetTask::hardReset()
{
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

    if (it != NULL)
    {
        const esp_partition_t *factory = esp_partition_get(it);
        esp_partition_iterator_release(it);
        if (esp_ota_set_boot_partition(factory) == ESP_OK)
        {
            LOGI("Factory resetting");
            if (configuration_.resetToDefaults())
            {
                LOGI("Configuration reset to defaults");
                esp_restart();
            }
            else
            {
                LOGE("Failed to reset configuration to defaults");
            }
        }
        else
        {
            LOGE("Failed to set boot partition");
        }
    }
    else
    {
        LOGE("Failed to find factory partition");
    }
}

void ResetTask::setMotorTask(MotorTask *motor_task)
{
    this->motor_task_ = motor_task;
}

void ResetTask::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void ResetTask::publishEvent(WiFiEvent event)
{
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}