#pragma once

#if SK_NETWORKING

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <vector>
#include <mqtt.h>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

#include <ElegantOTA.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>SETUP SMARTKNOB</title>
</head>
<body>
  <form>
    <label for="ssid">SSID:</label><br>
    <input type="text" id="ssid" name="ssid"><br>
    <label for="password">Password:</label><br>
    <input type="text" id="password" name="password"><br>
    <input type="submit" value="Continue">
  </form>
</body>
</html>
)rawliteral";

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
    void handleRoot();
    boolean captivePortal();
    void updateWifiState();
    void publishState(const ConnectivityState &state);
    char buf_[128];
    AsyncWebServer *server_;
};

class CaptivePortalHandler : public AsyncWebHandler
{
public:
    CaptivePortalHandler() {}
    virtual ~CaptivePortalHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        return request->url() == "/";
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        if (request->method() == HTTP_GET && request->url() == "/")
        {
            request->send(200, "text/html", index_html);
        }
        else
        {
            request->send(200, "text/html", index_html);
        }
    }
};

#else

class WifiTask
{
};

#endif
