#include <Arduino.h>

#include "configuration.h"
#include "display_task.h"
#include "error_handling_flow/reset_task.h"
#include "led_ring/led_ring_task.h"
#include "motor_foc/motor_task.h"
#include "network/mqtt_task.h"
#include "network/spotify/spotify_task.h"
#include "network/wifi_task.h"
#include "root_task.h"
#include "sensors/sensors_task.h"

#include "driver/temp_sensor.h"

#include "proto/serial_protocol_protobuf.h"
#include <logging.h>
#include <logging/adapters/freertos/free_rtos_adapter.h>
#include <logging/adapters/freertos/protocols/serial/serial_protocol_plaintext.h>

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !SK_FORCE_UART_STREAM
HWCDC stream_ = HWCDC();
#else
// TODO Check if uartstream works!!
UartStream stream_;
#endif

#if ENABLE_LOGGING
static SerialProtocolPlaintext serial_protocol(stream_);
static SerialProtocolPlaintext *serial_protocol_p = &serial_protocol;

static SerialProtocolProtobuf serial_protocol_protobuf(stream_);
static SerialProtocolProtobuf *serial_protocol_protobuf_p =
    &serial_protocol_protobuf;

static FreeRTOSAdapter adapter(&serial_protocol, xSemaphoreCreateMutex(),
                               "FreeRTOSAdapter", 1024 * 10, 0, 1);
static FreeRTOSAdapter *adapter_p = &adapter;
#else
static FreeRTOSAdapter *adapter_p = nullptr;
static SerialProtocolPlaintext *serial_protocol_p = nullptr;
static SerialProtocolProtobuf *serial_protocol_protobuf_p = nullptr;
#endif

Configuration config;

#if SK_DISPLAY
static DisplayTask display_task(0);
static DisplayTask *display_task_p = &display_task;
#else
static DisplayTask *display_task_p = nullptr;
#endif

#if SK_LEDS
static LedRingTask led_ring_task(1);
static LedRingTask *led_ring_task_p = &led_ring_task;
#else
static LedRingTask *led_ring_task_p = nullptr;
#endif

static MotorTask motor_task(1, config);

#if SK_WIFI
static WifiTask wifi_task(1, config);
static WifiTask *wifi_task_p = &wifi_task;
#else
static WifiTask *wifi_task_p = nullptr;

#endif

#if SK_MQTT
static MqttTask mqtt_task(1);
static MqttTask *mqtt_task_p = &mqtt_task;
#else
static MqttTask *mqtt_task_p = nullptr;

#endif

static SpotifyTask spotify_task(1, config);
static SpotifyTask *spotify_task_p = &spotify_task;

static SensorsTask sensors_task(1, &config);
static SensorsTask *sensors_task_p = &sensors_task;

static ResetTask reset_task(1, config);
static ResetTask *reset_task_p = &reset_task;

RootTask root_task(0, &config, motor_task, display_task_p, wifi_task_p,
                   mqtt_task_p, spotify_task_p, led_ring_task_p, sensors_task_p,
                   reset_task_p, adapter_p, serial_protocol_p,
                   serial_protocol_protobuf_p);

void initTempSensor()
{
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset =
        TSENS_DAC_L2; // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~
                      // 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

void setup()
{
#if ENABLE_LOGGING
    delay(100); // Delay to allow usb to connect before starting stream
    stream_.begin(MONITOR_SPEED);

    Logging::setAdapter(adapter_p);
#endif

    LOGI("Starting Seedlabs Smart Knob");

    initTempSensor();

    // TODO: move from eeprom to ffatfs
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        LOGE("Failed to start EEPROM");
    }

#if SK_DISPLAY
    // Create the task pinned to the specified core
    display_task.begin();

    // Connect display to motor_task's knob state feed
    root_task.addListener(display_task.getKnobStateQueue());

#endif

#if SK_LEDS
    led_ring_task_p->begin();
#endif

    root_task.begin();
    if (!config.loadPersistantConfigFromDisk())
    {
        config.savePersistantConfigToDisk();
    }

    if (!config.loadSettingsFromDisk())
    {
        config.saveSettingsToDisk();
    }

    if (config.loadSpotifyConfigFromDisk())
    {
        PB_SpotifyConfig spotify_config_ = config.getSpotifyConfig();
        if (strcmp(spotify_config_.access_token, "") != 0)
        {
            LOGV(LOG_LEVEL_DEBUG, "LOADED DEVICE ID %s", spotify_config_.device_id);
            spotify_task.begin();
            delay(10);
            spotify_task.setConfig(spotify_config_);
        }
    }

    root_task.loadConfiguration();

    motor_task.begin();

#if SK_WIFI
    wifi_task.addStateListener(root_task.getConnectivityStateQueue());
    wifi_task.begin();
#endif

#if SK_MQTT
    // IF WIFI CONNECTED CONNECT MQTT
    mqtt_task.addAppSyncListener(root_task.getAppSyncQueue());
    mqtt_task.begin();
#endif

    sensors_task_p->addStateListener(root_task.getSensorsStateQueue());
    sensors_task_p->begin();

    reset_task_p->begin();

    // Free up the Arduino loop task
    vTaskDelete(NULL);
}

void loop() {}
