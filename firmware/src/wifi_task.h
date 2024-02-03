#pragma once

#if SK_NETWORKING

#include <Arduino.h>
#include <WiFi.h>
#include <vector>
#include <mqtt.h>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"
#include <WebServer.h>
#include <ElegantOTA.h>

class WifiTask : public Task<WifiTask>
{
    friend class Task<WifiTask>; // Allow base Task to invoke protected run()

public:
    WifiTask(const uint8_t task_core);
    ~WifiTask();

    void setLogger(Logger *logger);
    void addStateListener(QueueHandle_t queue);

protected:
    void run();

private:
    std::vector<QueueHandle_t> state_listeners_;

    PB_SmartKnobState state_;
    SemaphoreHandle_t mutex_;
    Logger *logger_;
    void log(const char *msg);
    WiFiClient wifi_client;
    void setup_wifi();
    void publishState(const ConnectivityState &state);
    char buf_[128];
    WebServer *server_;
};

#else

class WifiTask
{
};

#endif
