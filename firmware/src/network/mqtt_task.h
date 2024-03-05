#pragma once

#if SK_MQTT

#include <PubSubClient.h>
#include <WiFi.h>
#include <vector>
#include <map>

#include "logger.h"
#include "task.h"
#include "cJSON.h"
#include "../app_config.h"
#include "../events/events.h"

class MqttTask : public Task<MqttTask>
{
    friend class Task<MqttTask>; // Allow base Task to invoke protected run()

public:
    MqttTask(const uint8_t task_core);
    ~MqttTask();

    QueueHandle_t getEntityStateReceivedQueue();

    void enqueueEntityStateToSend(EntityStateUpdate);
    void addAppSyncListener(QueueHandle_t queue);
    void setLogger(Logger *logger);
    void unlock();
    cJSON *getApps();
    void handleEvent(WiFiEvent event);
    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

    bool setup(MQTTConfiguration config);
    bool setupAndConnectNewCredentials(MQTTConfiguration config);
    bool reset();
    bool connect();
    bool disconnect();
    bool reconnect();
    bool init();

protected:
    void run();

private:
    std::map<std::string, EntityStateUpdate> entity_states_to_send;

    MQTTConfiguration config_;
    bool is_config_set;
    uint8_t retry_count = 0;

    QueueHandle_t entity_state_to_send_queue_;
    QueueHandle_t shared_events_queue;
    std::vector<QueueHandle_t> app_sync_listeners_;

    SemaphoreHandle_t mutex_app_sync_;
    WiFiClient wifi_client;
    PubSubClient mqtt_client;
    Logger *logger_;
    cJSON *apps;

    void log(const char *msg);

    void callback(char *topic, byte *payload, unsigned int length);

    void publishAppSync(const cJSON *state);

    void publishEvent(WiFiEvent event);

    void lock();
};

#else

class MqttTask
{
};

#endif
