// co-authored by carlhampuswall

#if SK_NETWORKING
#include "wifi_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "wifi_config.h"
#include "cJSON.h"
#include "root_task.h"

static const char *WIFI_TAG = "WIFI";

WifiTask::WifiTask(const uint8_t task_core) : Task{"wifi", 2048 * 2, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

WifiTask::~WifiTask()
{
    vSemaphoreDelete(mutex_);
}
void WifiTask::setup_wifi()
{
    const char *wifi_name = WIFI_SSID;
    const char *wifi_pass = WIFI_PASSWORD;

    WiFi.setHostname("SmartKnob");
    WiFi.setAutoReconnect(true);

    WiFi.begin(wifi_name, wifi_pass);

    uint8_t tries = 0;
    while (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_AP)
    {
        delay(1500);
        ESP_LOGD("NETWORKING", "WiFi connectien tries: %d", tries);
        if (tries > 0) // if we can't connect to wifi, start AP. tries dont really mean tries since we're not trying to connect to wifi here.
        {
            tries++;
            WiFi.mode(WIFI_AP);
            WiFi.softAP("SMARTKNOB-AP", "smartknob");
        }
    }
    updateWifiState();
}

void WifiTask::run()
{
    setup_wifi();
    WebServer server(80);
    server_ = &server;
    ElegantOTA.begin(server_);
    server_->on("/", [this]()
                { server_->send(200, "text/plain", "Welcome to the SmartKnob devkit (updated)"); });
    server_->begin();

    log("WebServer started");

    static uint32_t last_wifi_status;

    while (1)
    {
        // move webserver to a different task
        server_->handleClient();
        ElegantOTA.loop();

        if (millis() - last_wifi_status > 5000)
        {
            updateWifiState();
            last_wifi_status = millis();
        }

        delay(5);
    }
}

void WifiTask::updateWifiState()
{
    int8_t rssi = WiFi.RSSI();
    uint8_t signal_strenth_status;

    if (rssi < -110)
    {
        signal_strenth_status = 4;
    }
    else if (rssi < -100)
    {
        signal_strenth_status = 3;
    }
    else if (rssi < -85)
    {
        signal_strenth_status = 2;
    }
    else if (rssi < -70)
    {
        signal_strenth_status = 1;
    }
    else
    {
        signal_strenth_status = 0;
    }

    // harvest state;
    ConnectivityState state = {
        WiFi.isConnected(),
        WiFi.RSSI(),
        signal_strenth_status,
        WIFI_SSID,
        WiFi.localIP().toString().c_str()};

    publishState(state);
}

void WifiTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void WifiTask::publishState(const ConnectivityState &state)
{
    for (auto listener : state_listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

void WifiTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void WifiTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif
