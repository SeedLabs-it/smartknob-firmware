#if SK_WIFI
#include "wifi_task.h"
#include "semaphore_guard.h"
#include "util.h"
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
    const char *wifi_name = "WIFI_SSID";
    const char *wifi_pass = "WIFI_PASSWORD";

    // WiFi.persistent(SK_REMEMBER_WIFI);
    WiFi.setHostname("SmartKnob");
    WiFi.setAutoReconnect(true);

    uint8_t tries = 0;

    while (WiFi.waitForConnectResult() != WL_CONNECTED && tries < 3) //! UP TRIES WHEN IN PRODUCTION
    {
        ESP_LOGD(WIFI_TAG, "Connecting to wifi, attempt: %d", tries);
        // WiFi.begin("Fam Wall", "WallsNetwork989303"); ok so it does store in eeprom by default now as long as persistent is set to true
        WiFi.begin();
        delay(3000);
        tries++;
    }

    if (tries >= 3)
    {
        ESP_LOGD(WIFI_TAG, "Failed to connect to wifi, starting AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("SMARTKNOB-AP", "smartknob");

        while (WiFi.softAPgetStationNum() == 0)
        {
            ESP_LOGD(WIFI_TAG, "Waiting for client to connect to AP");
            delay(1000);
        }

        ESP_LOGD(WIFI_TAG, "Client connected to AP");
    }

    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        ESP_LOGD(WIFI_TAG, "Connected as: %s", WiFi.localIP().toString().c_str());
    }
    updateWifiState();
}

void WifiTask::run()
{

    setup_wifi();

    server_ = new WebServer(80);

    server_->on("/", HTTP_GET, [this]()
                { 
                    if (WiFi.isConnected()) {
                        server_->sendHeader("Location", "/mqtt");
                        server_->send(302, "text/plain", "Connected to WiFi redirecting to MQTT setup!");    
                    } else {
                         server_->send(200, "text/html", "<form action='/submit' method='get'>"
                                                  "SSID: <input type='text' name='ssid'><br>"
                                                  "Password: <input type='text' name='password'><br>"
                                                  "<input type='hidden' name='setup_type' value='wifi'>"
                                                  "<input type='submit' value='Submit'>"
                                                  "</form>");
                    } });

    server_->on("/mqtt", HTTP_GET, [this]()
                { server_->send(200, "text/html", "<form action='/submit' method='get'>"
                                                  "MQTT SERVER: <input type='text' name='mqtt_server'><br>"
                                                  "MQTT PORT: <input type='number' name='mqtt_port'><br>"
                                                  "MQTT USER: <input type='text' name='mqtt_user'><br>"
                                                  "MQTT PASSWORD: <input type='text' name='mqtt_password'><br>"
                                                  "<input type='hidden' name='setup_type' value='mqtt'>"
                                                  "<input type='submit' value='Submit'>"
                                                  "</form>"); });
    server_->on("/submit", [this]()
                {
        if (server_->arg("setup_type") == "wifi")
        {
            String name = server_->arg("ssid");
            String age = server_->arg("password");

            WiFi.begin(name.c_str(), age.c_str());

            uint8_t tries = 0;

            while (WiFi.waitForConnectResult() != WL_CONNECTED && tries < 5) //! UP TRIES WHEN IN PRODUCTION
            {
                ESP_LOGD(WIFI_TAG, "Connecting to wifi, attempt: %d", tries);
                // WiFi.begin("Fam Wall", "WallsNetwork989303"); ok so it does store in eeprom by default now as long as persistent is set to true
                // WiFi.begin();
                delay(3000);
                tries++;
            }

            if (tries >= 5)
            {
                server_->sendHeader("Location", "/");
                server_->send(302, "text/plain", "Invalid WiFi credentials!");
            }

            server_->sendHeader("Location", "/mqtt");
            server_->send(302, "text/plain", "Connected to WiFi redirecting to MQTT setup!");
        }
        else if (server_->arg("setup_type") == "mqtt")
        {
            String mqtt_server = server_->arg("mqtt_server");
            uint32_t mqtt_port = server_->arg("mqtt_port").toInt();
            String mqtt_user = server_->arg("mqtt_user");
            String mqtt_password = server_->arg("mqtt_password");

            preferences.begin("mqtt", false); 
            preferences.clear();

            preferences.putString("mqtt_server", mqtt_server);
            preferences.putUInt("mqtt_port", mqtt_port);
            preferences.putString("mqtt_user", mqtt_user);
            preferences.putString("mqtt_password", mqtt_password);

            preferences.end();

            WiFi.mode(WIFI_STA);
            WiFi.softAPdisconnect(true);
            server_->send(200, "text/html", "MQTT setup complete!");
        } });

    server_->begin();

    log("WebServer started");

    static uint32_t last_wifi_status;

    while (1)
    {
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
        WiFi.SSID().c_str(),
        WiFi.localIP().toString().c_str(),
        WiFi.getMode() == WIFI_AP ? true : false,
        WiFi.softAPIP(),
        WiFi.softAPgetStationNum() > 0 ? true : false,
    };

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
