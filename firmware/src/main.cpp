#include <Arduino.h>

#include "configuration.h"
#include "display_task.h"
#include "app_task.h"
#include "motor_task.h"
#include "networking_task.h"
#include "sensors_task.h"
#include "led_ring_task.h"

Configuration config;

#if SK_DISPLAY
static DisplayTask display_task(0);
static DisplayTask *display_task_p = &display_task;
#else
static DisplayTask *display_task_p = nullptr;
#endif

#if SK_LEDS
static LedRingTask led_ring_task(0);
static LedRingTask *led_ring_task_p = &led_ring_task;
#else
static LedRingTask *led_ring_task_p = nullptr;
#endif

static MotorTask motor_task(1, config);

#if SK_NETWORKING
static NetworkingTask networking_task(1);
static NetworkingTask *networking_task_p = &networking_task;

#else
static NetworkingTask *networking_task_p = nullptr;

#endif

#if SK_SEEDLABS_DEVKIT
static SensorsTask sensors_task(1);
static SensorsTask *sensors_task_p = &sensors_task;
#else
static SensorsTask *sensors_task_p = nullptr;

#endif

AppTask app_task(0, motor_task, display_task_p, networking_task_p, led_ring_task_p, sensors_task_p);

void setup()
{
#if SK_DISPLAY
    display_task.setLogger(&app_task);
    display_task.begin();

    // Connect display to motor_task's knob state feed
    app_task.addListener(display_task.getKnobStateQueue());

    // link apps from display task
    app_task.setOnboardingApps(display_task.getOnboarding());
    app_task.setHassApps(display_task.getHassApps());

#endif

#if SK_LEDS
    led_ring_task_p->begin();
#endif

    // TODO: wait for display task init finishes
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    app_task.begin();

    config.setLogger(&app_task);
    if (!config.loadFromDisk())
    {
        config.saveToDisk();
    }

    app_task.setConfiguration(&config);

    motor_task.setLogger(&app_task);
    motor_task.begin();

#if SK_NETWORKING
    networking_task.setLogger(&app_task);
    networking_task.addStateListener(app_task.getConnectivityStateQueue());
    networking_task.addAppSyncListener(app_task.getAppSyncQueue());
    networking_task.begin();
#endif

#if SK_SEEDLABS_DEVKIT
    sensors_task_p->setLogger(&app_task);
    sensors_task_p->addStateListener(app_task.getSensorsStateQueue());
    sensors_task_p->begin();
#endif

    // Free up the Arduino loop task
    vTaskDelete(NULL);
}

void loop()
{
    // char buf[50];
    // static uint32_t last_stack_debug;
    // if (millis() - last_stack_debug > 1000) {
    //   interface_task.log("Stack high water:");
    //   snprintf(buf, sizeof(buf), "  main: %d", uxTaskGetStackHighWaterMark(NULL));
    //   interface_task.log(buf);
    //   #if SK_DISPLAY
    //     snprintf(buf, sizeof(buf), "  display: %d", uxTaskGetStackHighWaterMark(display_task.getHandle()));
    //     interface_task.log(buf);
    //   #endif
    //   snprintf(buf, sizeof(buf), "  motor: %d", uxTaskGetStackHighWaterMark(motor_task.getHandle()));
    //   interface_task.log(buf);
    //   snprintf(buf, sizeof(buf), "  interface: %d", uxTaskGetStackHighWaterMark(interface_task.getHandle()));
    //   interface_task.log(buf);
    //   snprintf(buf, sizeof(buf), "Heap -- free: %d, largest: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    //   interface_task.log(buf);
    //   last_stack_debug = millis();
    // }
}