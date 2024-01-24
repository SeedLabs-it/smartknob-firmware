#pragma once

#include <AceButton.h>
#include <Arduino.h>

#include "configuration.h"
#include "display_task.h"
#include "logger.h"
#include "motor_task.h"
#include "serial/serial_protocol_plaintext.h"
#include "serial/serial_protocol_protobuf.h"
#include "serial/uart_stream.h"
#include "task.h"
#include "app_config.h"
#include "networking_task.h"
#include "led_ring_task.h"
#include "sensors_task.h"

class AppTask : public Task<AppTask>,
                public Logger
{

    friend class Task<AppTask>; // Allow base Task to invoke protected run()

public:
    AppTask(const uint8_t task_core, MotorTask &motor_task, DisplayTask *display_task, NetworkingTask *networking_task, LedRingTask *led_ring_task, SensorsTask *sensors_task);
    virtual ~AppTask();

    void log(const char *msg) override;
    void setConfiguration(Configuration *configuration);

    void setApps(Apps *apps);

    void addListener(QueueHandle_t queue);

    QueueHandle_t getConnectivityStateQueue();
    QueueHandle_t getSensorsStateQueue();
    QueueHandle_t getAppSyncQueue();

    void strainCalibrationCallback();
    void verboseToggleCallback();

protected:
    void run();

private:
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !SK_FORCE_UART_STREAM
    HWCDC stream_;
#else
    UartStream stream_;
#endif
    MotorTask &motor_task_;
    DisplayTask *display_task_;
    NetworkingTask *networking_task_;
    Apps *apps;
    LedRingTask *led_ring_task_;
    SensorsTask *sensors_task_;
    char buf_[128];

    std::vector<QueueHandle_t> listeners_;

    SemaphoreHandle_t mutex_;
    Configuration *configuration_ = nullptr; // protected by mutex_

    PB_PersistentConfiguration configuration_value_;
    bool configuration_loaded_ = false;

    uint8_t strain_calibration_step_ = 0;
    int32_t strain_reading_ = 0;

    SerialProtocol *current_protocol_ = nullptr;
    bool remote_controlled_ = false;
    int current_config_ = 0;
    uint8_t press_count_ = 1;

    uint8_t last_strain_pressed_played_ = VIRTUAL_BUTTON_IDLE;

    PB_SmartKnobState latest_state_ = {};
    PB_SmartKnobConfig latest_config_ = {};

    ConnectivityState latest_connectivity_state_ = {};
    SensorsState latest_sensors_state_ = {};

    cJSON *apps_ = NULL;

    QueueHandle_t log_queue_;
    QueueHandle_t knob_state_queue_;

    QueueHandle_t connectivity_status_queue_;
    QueueHandle_t sensors_status_queue_;

    QueueHandle_t app_sync_queue_;

    SerialProtocolPlaintext plaintext_protocol_;
    SerialProtocolProtobuf proto_protocol_;

    void changeConfig(int8_t id);
    void updateHardware(AppState app_state);
    void publishState();
    void applyConfig(PB_SmartKnobConfig config, bool from_remote);
    void publish(const AppState &state);
};
