#pragma once

#if SK_WIFI

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>
#include <WebServer.h>
#include <vector>
#include <Preferences.h>

#include "../proto/proto_gen/smartknob.pb.h"
#include "../task.h"
#include "../app_config.h"

#include "../events/events.h"
#include "../notify/wifi_notifier/wifi_notifier.h"

#include "./spotify/spotify_api.h"

#if SK_ELEGANTOTA_PRO
#include <ElegantOTAPro.h>
#else
#include <ElegantOTA.h>
#endif

class WifiTask : public Task<WifiTask>
{
    friend class Task<WifiTask>; // Allow base Task to invoke protected run()

public:
    WifiTask(const uint8_t task_core, Configuration &configuration);
    ~WifiTask();

    void addStateListener(QueueHandle_t queue);

    WiFiNotifier *getNotifier();
    QueueHandle_t getWiFiEventsQueue();
    void handleCommand(WiFiCommand command);

    void mqttConnected(bool connected);
    void retryMqtt(bool retry);

    void resetRetryCount();

    void setConfig(WiFiConfiguration config);

protected:
    void run();

private:
    WiFiConfiguration wifi_config_;
    bool is_config_set;

    Configuration &configuration_;

    bool mqtt_connected;
    bool retry_mqtt;

    uint8_t retry_count = 0;

    std::vector<QueueHandle_t> state_listeners_;

    PB_SmartKnobState state_;
    SemaphoreHandle_t mutex_;
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
    bool tryNewCredentialsWiFiSTA(WiFiConfiguration wifi_config);

    WiFiNotifier wifi_notifier;
    // websrver handlers
    void webHandlerWiFiForm();
    void webHandlerMQTTForm();
    void webHandlerWiFiCredentials();
    void webHandlerMQTTCredentials();
    void webHandlerSpotifyCredentials();

    void downloadConfig();

    char redirect_page[32] = "done";
};

#else

class WifiTask
{
};

#endif
