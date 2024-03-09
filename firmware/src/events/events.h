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
    uint8_t retry_count;
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

struct WiFiError
{
    uint8_t retry_count;
};

struct MQTTError
{
    uint8_t retry_count;
};

enum ErrorType
{
    NO_ERROR,
    WIFI_ERROR,
    MQTT_ERROR
};

union ErrorBody
{
    WiFiError wifi_error;
    MQTTError mqtt_error;
};

struct Error
{
    ErrorType type;
    ErrorBody body;
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
    Error error;
};

// TODO, think events more careful, for example add SK_MQTT_CREDENTIALS_RECIEVED
// TODO add uniq prefix, clashing with some events
enum EventType
{
    SK_WIFI_AP_STARTED = 1,
    SK_WIFI_STATUS,
    SK_AP_CLIENT,
    SK_WEB_CLIENT,
    SK_WIFI_STA_TRY_NEW_CREDENTIALS,
    SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED,
    SK_WIFI_STA_CONNECTING,
    SK_WIFI_STA_CONNECTED,
    SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS,
    SK_WIFI_STA_CONNECTION_FAILED,
    SK_WIFI_STA_RETRY_LIMIT_REACHED,
    SK_WEB_CLIENT_MQTT,
    SK_MQTT_TRY_NEW_CREDENTIALS,
    SK_MQTT_TRY_NEW_CREDENTIALS_FAILED,
    SK_MQTT_NEW_CREDENTIALS_RECIEVED,
    SK_MQTT_CONNECTING,
    SK_MQTT_SETUP,
    SK_MQTT_RESET,
    SK_MQTT_RETRY_LIMIT_REACHED,
    SK_MQTT_INIT,
    SK_MQTT_CONNECTION_FAILED,
    SK_MQTT_STATE_UPDATE,
    SK_MQTT_CONNECTED,
    SK_MQTT_CONNECTED_NEW_CREDENTIALS,
    SK_RESET_ERROR,
    SK_DISMISS_ERROR,

    SK_MQTT_ERROR,
    SK_WIFI_ERROR,
};

// TODO: rename to generic event?
struct WiFiEvent
{
    EventType type;
    WiFiEventBody body;
    uint32_t sent_at = millis();
};