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
#include "notify/mqtt_notifier/mqtt_notifier.h"

// struct EntityStateId
// {
//     std::string entity_id;
//     std::string app_id;
// };
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
    void handleCommand(MqttCommand command);
    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

    bool setup(MQTTConfiguration config);

    bool reset();
    bool connect();
    bool disconnect();
    bool reconnect();
    bool init();

    MqttNotifier *getNotifier();

    void setConfig(MQTTConfiguration config);

protected:
    void run();

private:
    std::map<std::string, EntityStateUpdate> entity_states_to_send;

    std::map<std::string, std::string> unacknowledged_ids;
    std::map<std::string, EntityStateUpdate> unacknowledged_states;

    MQTTConfiguration config_;
    bool is_config_set;
    uint8_t retry_count = 0;

    bool hass_init_acknowledged = false;

    QueueHandle_t entity_state_to_send_queue_;
    QueueHandle_t shared_events_queue;
    std::vector<QueueHandle_t> app_sync_listeners_;

    SemaphoreHandle_t mutex_app_sync_;
    WiFiClient wifi_client;
    PubSubClient mqtt_client;
    Logger *logger_;
    cJSON *apps;

    MqttNotifier mqtt_notifier;

    std::string generatePayloadId();

    void log(const char *msg);

    void callback(char *topic, byte *payload, unsigned int length);

    void publishAppSync(const cJSON *state);

    void publishEvent(WiFiEvent event);

    bool setupAndConnectNewCredentials(MQTTConfiguration config);

    void lock();
};

#else

class MqttTask
{
};

#endif
