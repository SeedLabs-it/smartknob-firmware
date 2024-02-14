#pragma once

#if SK_MQTT

#include <PubSubClient.h>
#include <WiFi.h>
#include <vector>
#include <map>
#include <Preferences.h>

#include "logger.h"
#include "task.h"
#include "cJSON.h"
#include "app_config.h"
#include "events/events.h"

struct MQTTConfig
{
    std::string host;
    uint16_t port;
    std::string user;
    std::string password;
};

class MqttTask : public Task<MqttTask>
{
    friend class Task<MqttTask>; // Allow base Task to invoke protected run()

public:
    MqttTask(const uint8_t task_core);
    ~MqttTask();

    QueueHandle_t getConnectivityStateQueue();
    QueueHandle_t getEntityStateReceivedQueue();

    void addStateListener(QueueHandle_t queue);

    void enqueueEntityStateToSend(EntityStateUpdate);
    void addAppSyncListener(QueueHandle_t queue);
    void setLogger(Logger *logger);
    void unlock();
    cJSON *getApps();
    void handleEvent(WiFiEvent event);
    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

protected:
    void run();

private:
    const char *mqtt_server;
    uint32_t mqtt_port;
    const char *mqtt_user;
    const char *mqtt_password;

    std::vector<QueueHandle_t> state_listeners_;

    QueueHandle_t connectivity_status_queue_;
    QueueHandle_t entity_state_to_send_queue_;
    QueueHandle_t shared_events_queue;
    // QueueHandle_t entity_state_received_queue_;
    std::vector<QueueHandle_t> app_sync_listeners_;

    SemaphoreHandle_t mutex_app_sync_;
    WiFiClient wifi_client;
    PubSubClient mqttClient;
    Logger *logger_;
    cJSON *apps;

    Preferences preferences;

    ConnectivityState last_connectivity_state_;
    MqttState mqtt_state_;

    void publishState(const MqttState &state);

    void log(const char *msg);

    void setup_mqtt(MQTTConfig config);
    void reconnect_mqtt();
    void callback_mqtt(char *topic, byte *payload, unsigned int length);

    void publishAppSync(const cJSON *state);

    void publishEvent(WiFiEvent event);

    void lock();
};

#else

class MqttTask
{
};

#endif
