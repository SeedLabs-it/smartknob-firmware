#include <Arduino.h>

#include "configuration.h"
#include "display_task.h"
#include "root_task.h"
#include "motor_foc/motor_task.h"
#include "network/wifi_task.h"
#include "sensors/sensors_task.h"
#include "error_handling_flow/reset_task.h"
#include "led_ring/led_ring_task.h"

#include "driver/temp_sensor.h"

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
static WifiTask wifi_task(1);
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

static SensorsTask sensors_task(1, &config);
static SensorsTask *sensors_task_p = &sensors_task;

static ResetTask reset_task(1, config);
static ResetTask *reset_task_p = &reset_task;

RootTask root_task(0, &config, motor_task, display_task_p, wifi_task_p, mqtt_task_p, led_ring_task_p, sensors_task_p, reset_task_p);

void initTempSensor()
{
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2; // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

void setup()
{

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

    // TODO: remove this. Wait for display task init finishes
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    root_task.begin();
    if (!config.loadFromDisk())
    {
        config.saveToDisk();
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

void loop()
{
}