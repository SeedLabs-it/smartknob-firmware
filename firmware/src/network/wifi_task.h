#pragma once

#if SK_WIFI

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <vector>
#include <mqtt.h>
#include <Preferences.h>

#include "../logger.h"
#include "../proto_gen/smartknob.pb.h"
#include "../task.h"
#include "../app_config.h"

#include "../events/events.h"
#include "../notify/wifi_notifier/wifi_notifier.h"

#include <ElegantOTA.h>

class WifiTask : public Task<WifiTask>
{
    friend class Task<WifiTask>; // Allow base Task to invoke protected run()

public:
    WifiTask(const uint8_t task_core);
    ~WifiTask();

    void setLogger(Logger *logger);
    void addStateListener(QueueHandle_t queue);

    WiFiNotifier *getNotifier();
    QueueHandle_t getWiFiEventsQueue();
    void handleCommand(WiFiCommand command);

protected:
    void run();

private:
    std::vector<QueueHandle_t> state_listeners_;

    PB_SmartKnobState state_;
    SemaphoreHandle_t mutex_;
    Logger *logger_;
    void log(const char *msg);
    WiFiClient wifi_client;
    void updateWifiState();
    void publishState(const ConnectivityState &state);
    char buf_[128];
    WebServer *server_;
    Preferences preferences;

    void publishWiFiEvent(WiFiEvent event);
    void startWebServer();
    bool is_webserver_started = false;
    void startWiFiAP();
    bool startWiFiSTA(WiFiConfiguration wifi_config);
    bool startNewWiFiSTA(WiFiConfiguration wifi_config);

    WiFiNotifier wifi_notifier;
    // websrver handlers
    void webHandlerWiFiForm();
    void webHandlerMQTTForm();
    void webHandlerWiFiCredentials();
    void webHandlerMQTTCredentials();
};

#else

class WifiTask
{
};

#endif
