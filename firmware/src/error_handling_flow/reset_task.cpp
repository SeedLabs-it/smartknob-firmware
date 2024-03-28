#include "reset_task.h"

#define RESET_BUTTON GPIO_NUM_0

ResetTask::ResetTask(const uint8_t task_core, Configuration &configuration) : Task("ResetTask", 2048, task_core), configuration_(configuration)
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
    uint8_t pressedCount = 0;

    while (1)
    {
        // Button is not pressed.
        if (gpio_get_level(RESET_BUTTON) == 1)
        {
            reset_button_pressed = millis();
            if (held)
            {
                pressedCount += 1;
                held = false;
                ESP_LOGD("", "Button pressed %d times", pressedCount);
            }
        }
        else // Button is pressed when low (0)
        {
            reset_button_released = millis();
            if (!held)
            {
                held = true;
                ESP_LOGD("", "Button held");
            }

            if (millis() - reset_button_pressed > RESET_SECONDS * 1000)
            {
                log("Resetting to factory defaults");
                if (configuration_.resetToDefaults())
                {
                    log("Configuration reset to defaults");
                    esp_restart();
                }
                else
                {
                    log("Failed to reset configuration to defaults");
                }
            }
        }

        if (pressedCount >= 6)
        {
            esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

            if (it != NULL)
            {
                const esp_partition_t *factory = esp_partition_get(it);
                esp_partition_iterator_release(it);
                if (esp_ota_set_boot_partition(factory) == ESP_OK)
                {
                    log("FACTORY RESETTING");
                    if (configuration_.resetToDefaults())
                    {
                        log("Configuration reset to defaults");
                        esp_restart();
                    }
                    else
                    {
                        log("Failed to reset configuration to defaults");
                    }
                }
                else
                {
                    if (verbose_)
                    {
                        log("Failed to set boot partition");
                    }
                }
            }
            else
            {
                if (verbose_)
                {
                    log("Failed to find factory partition");
                }
            }
        }

        if (millis() - reset_button_released > 500) // Reset the pressed count after 500ms of no button press
        {
            pressedCount = 0;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void ResetTask::toggleVerbose()
{
    verbose_ = !verbose_;
}

void ResetTask::setVerbose(bool verbose)
{
    verbose_ = verbose;
}

void ResetTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void ResetTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        sprintf(buf_, "ResetTask: %s", msg);
        logger_->log(buf_);
    }
}