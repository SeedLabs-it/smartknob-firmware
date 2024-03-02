#pragma once

#include <stdint.h>

struct WiFiAPStarted
{
    char ssid[128];
    char passphrase[128];
};

struct WiFiSTAConnecting
{
    char ssid[128];
    char passphrase[128];
    uint8_t tick;
};

struct WiFiStatus
{
    /* data */
    bool is_ap;
    bool connected;
    uint8_t signal_strenth_status;
    int8_t rssi;
};

struct APClient
{
    bool connected;
};

struct WebClient
{
    bool connected;
};

// Idea if we would need to get info async, settings
// struct SummaryStatus
// {
//     uint8_t sta_status; // 0 disabled, 1 connected, -1 disconnected
//     uint8_t ap_status;  // 0 disabled, 1 client connected, -1 client disconnected
//     uint8_t sta_signal_strenth_status;
//     int8_t sta_rssi;
// };

struct MQTTStateUpdate
{
    char *app_id;
    cJSON *state;
};

union WiFiEventBody
{
    WiFiAPStarted wifi_ap_started;
    WiFiStatus wifi_status;
    APClient ap_client;
    WebClient web_client;
    WiFiSTAConnecting wifi_sta_connecting;
    WiFiSTAConnecting wifi_sta_connected;
    MQTTConfiguration mqtt_connecting;
    MQTTStateUpdate mqtt_state_update;
};

// TODO, think events more careful, for example add MQTT_CREDENTIALS_RECIEVED
// TODO add uniq prefix, clashing with some events
enum EventType
{
    WIFI_AP_STARTED = 1,
    WIFI_STATUS,
    AP_CLIENT,
    WEB_CLIENT,
    WIFI_STA_CONNECTING,
    WIFI_STA_CONNECTED,
    WIFI_STA_CONNECTED_NEW_CREDENTIALS,
    WIFI_STA_CONNECTION_FAILED,
    WEB_CLIENT_MQTT,
    MQTT_CREDENTIALS_RECIEVED,
    MQTT_CONNECTING,
    MQTT_SETUP,
    MQTT_RESET,
    MQTT_RETRY_LIMIT_REACHED,
    MQTT_INIT,
    MQTT_CONNECTION_FAILED,
    MQTT_STATE_UPDATE,
    SK_MQTT_CONNECTED
};

// TODO: rename to generic event?
struct WiFiEvent
{
    EventType type;
    WiFiEventBody body;
};