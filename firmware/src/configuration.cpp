#include <FFat.h>

#include "pb_decode.h"
#include "pb_encode.h"

#include "proto_gen/smartknob.pb.h"
#include "semaphore_guard.h"

#include "configuration.h"

static const char *CONFIG_PATH = "/config.pb";
static const char *SETTINGS_PATH = "/settings.pb";

Configuration::Configuration()
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    wifi_config = WiFiConfiguration();
    mqtt_config = MQTTConfiguration();
    loadOSConfiguration();
    loadWiFiConfiguration();
    loadMQTTConfiguration();

    std::string mac_address = std::string(WiFi.macAddress().c_str());
    mac_address.erase(
        std::remove_if(
            mac_address.begin(),
            mac_address.end(),
            [](char c)
            {
                return c == ':';
            }),
        mac_address.end());

    sprintf(wifi_config.knob_id, "%s", std::string("SKDK_" + mac_address.substr(mac_address.length() - 6)).c_str());
    sprintf(mqtt_config.knob_id, "%s", wifi_config.knob_id);
}

Configuration::~Configuration()
{
    vSemaphoreDelete(mutex_);
}

const char *Configuration::getKnobId()
{
    return wifi_config.knob_id;
}

bool Configuration::loadFromDisk()
{
    SemaphoreGuard lock(mutex_);
    FatGuard fatGuard;
    if (!fatGuard.mounted_)
    {
        return false;
    }

    File f = FFat.open(CONFIG_PATH);
    if (!f)
    {
        LOGE("Failed to read config file");
        return false;
    }

    size_t read = f.readBytes((char *)pb_stream_buffer_, sizeof(pb_stream_buffer_));
    f.close();

    pb_istream_t stream = pb_istream_from_buffer(pb_stream_buffer_, read);
    if (!pb_decode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_))
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Decoding config failed: %s", PB_GET_ERROR(&stream));
        LOGE(buf_);
        pb_buffer_ = {};
        return false;
    }

    if (pb_buffer_.version != PERSISTENT_CONFIGURATION_VERSION)
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Invalid config version. Expected %u, received %u", PERSISTENT_CONFIGURATION_VERSION, pb_buffer_.version);
        LOGE(buf_);
        pb_buffer_ = {};
        return false;
    }
    loaded_ = true;

    char buf_[200];
    snprintf(
        buf_,
        sizeof(buf_),
        "Motor calibration: calib=%u, pole_pairs=%u, zero_offset=%.2f, cw=%u",
        pb_buffer_.motor.calibrated,
        pb_buffer_.motor.pole_pairs,
        pb_buffer_.motor.zero_electrical_offset,
        pb_buffer_.motor.direction_cw);
    LOGI(buf_);

    return true;
}

bool Configuration::saveToDisk()
{
    {
        SemaphoreGuard lock(mutex_);

        pb_ostream_t stream = pb_ostream_from_buffer(pb_stream_buffer_, sizeof(pb_stream_buffer_));
        pb_buffer_.version = PERSISTENT_CONFIGURATION_VERSION;
        if (!pb_encode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_))
        {
            char buf_[200];
            snprintf(buf_, sizeof(buf_), "Encoding failed: %s", PB_GET_ERROR(&stream));
            LOGE(buf_);
            return false;
        }

        FatGuard fatGuard;
        if (!fatGuard.mounted_)
        {
            return false;
        }

        File f = FFat.open(CONFIG_PATH, FILE_WRITE);
        if (!f)
        {
            LOGE("Failed to read config file");
            return false;
        }

        size_t written = f.write(pb_stream_buffer_, stream.bytes_written);
        f.close();

        LOGD("Saved config. Wrote %d bytes", written);

        if (written != stream.bytes_written)
        {
            LOGE("Failed to write all bytes to file");
            return false;
        }
    }

    if (shared_events_queue != NULL)
    {
        WiFiEvent event;
        event.type = SK_CONFIGURATION_SAVED;
        publishEvent(event);
    }

    return true;
}

bool Configuration::loadSettingsFromDisk()
{
    SemaphoreGuard lock(mutex_);
    FatGuard fatGuard;
    if (!fatGuard.mounted_)
    {
        return false;
    }

    File f = FFat.open(SETTINGS_PATH);
    if (!f)
    {
        LOGE("Failed to read settings file");
        return false;
    }

    size_t read = f.readBytes((char *)settings_stream_buffer_, sizeof(settings_stream_buffer_));
    f.close();

    pb_istream_t stream = pb_istream_from_buffer(settings_stream_buffer_, read);
    if (!pb_decode(&stream, SETTINGS_Settings_fields, &settings_buffer_))
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Decoding settings failed: %s", PB_GET_ERROR(&stream));
        LOGE(buf_);
        settings_buffer_ = {};
        return false;
    }

    if (settings_buffer_.protocol_version != SETTINGS_VERSION)
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Invalid config version. Expected %u, received %u", SETTINGS_VERSION, settings_buffer_.protocol_version);
        LOGE(buf_);
        settings_buffer_ = {};
        return false;
    }
    settings_loaded_ = true;

    return true;
}

bool Configuration::saveSettingsToDisk()
{
    {
        SemaphoreGuard lock(mutex_);

        pb_ostream_t stream = pb_ostream_from_buffer(settings_stream_buffer_, sizeof(settings_stream_buffer_));
        settings_buffer_.protocol_version = SETTINGS_VERSION;
        if (!pb_encode(&stream, SETTINGS_Settings_fields, &settings_buffer_))
        {
            char buf_[200];
            snprintf(buf_, sizeof(buf_), "Encoding failed: %s", PB_GET_ERROR(&stream));
            LOGE(buf_);
            return false;
        }

        FatGuard fatGuard;
        if (!fatGuard.mounted_)
        {
            return false;
        }

        File f = FFat.open(SETTINGS_PATH, FILE_WRITE);
        if (!f)
        {
            LOGE("Failed to read settings file");
            return false;
        }

        size_t written = f.write(settings_stream_buffer_, stream.bytes_written);
        f.close();

        LOGD("Saved settings. Wrote %d bytes", written);

        if (written != stream.bytes_written)
        {
            LOGE("Failed to write all bytes to settings file");
            return false;
        }
    }

    // if (shared_events_queue != NULL)
    // {
    //     WiFiEvent event;
    //     event.type = SK_CONFIGURATION_SAVED;
    //     publishEvent(event);
    // }

    return true;
}

bool Configuration::setSettings(SETTINGS_Settings &settings)
{
    {
        SemaphoreGuard lock(mutex_);
        settings_buffer_ = settings;

        if (shared_events_queue != NULL)
        {
            WiFiEvent event;
            event.type = SK_SETTINGS_CHANGED;
            publishEvent(event);
        }
    }
    return saveSettingsToDisk();
    // return true;
}

// bool Configuration::resetSettingsToDefaults()
// {
//     {
//         SemaphoreGuard lock(mutex_);
//         settings_buffer_ = default_settings;
//         // settings_loaded_ = true;
//     }
//     return true;
//     // return saveSettingsToDisk();
// }

SETTINGS_Settings Configuration::getSettings()
{
    SemaphoreGuard lock(mutex_);
    if (!settings_loaded_)
    {
        LOGD("Settings not loaded, loading default settings instead.");
        settings_buffer_ = default_settings;
        settings_loaded_ = true;
        return settings_buffer_; // RETURN ACTUAL DEFAULTS
    }
    LOGV(PB_LogLevel_DEBUG, "Settings loaded.");
    return settings_buffer_;
}

bool Configuration::resetToDefaults()
{
    EEPROM.put(WIFI_SET_EEPROM_POS, false);
    EEPROM.put(MQTT_SET_EEPROM_POS, false);
    EEPROM.put(OS_MODE_EEPROM_POS, OSMode::ONBOARDING);
    EEPROM.commit();
    return true;
}

bool Configuration::saveWiFiConfiguration(WiFiConfiguration wifi_config_to_save)
{
    // TODO: persist in a file
    char buf_[512];
    sprintf(buf_, "Saving wifi credentials %s %s", wifi_config_to_save.ssid, wifi_config_to_save.passphrase);
    LOGD(buf_);

    is_wifi_set = true;
    EEPROM.put(WIFI_SSID_EEPROM_POS, wifi_config_to_save.ssid);
    EEPROM.put(WIFI_PASSPHRASE_EEPROM_POS, wifi_config_to_save.passphrase);
    EEPROM.put(WIFI_SET_EEPROM_POS, is_wifi_set);

    return EEPROM.commit();
}

WiFiConfiguration Configuration::getWiFiConfiguration()
{
    return wifi_config;
}

bool Configuration::loadWiFiConfiguration()
{

    char buf_[512];

    EEPROM.get(WIFI_SSID_EEPROM_POS, wifi_config.ssid);
    EEPROM.get(WIFI_PASSPHRASE_EEPROM_POS, wifi_config.passphrase);
    EEPROM.get(WIFI_SET_EEPROM_POS, is_wifi_set);

    sprintf(buf_, "loaded wifi credentials %s %s %d", wifi_config.ssid, wifi_config.passphrase, is_wifi_set);
    LOGD(buf_);

    return is_wifi_set;
}

bool Configuration::saveMQTTConfiguration(MQTTConfiguration mqtt_config_to_save)
{
    // TODO: persist in a file
    char buf_[512];
    sprintf(buf_, "saving MQTT credentials %s %d %s %s", mqtt_config_to_save.host, mqtt_config_to_save.port, mqtt_config_to_save.user, mqtt_config_to_save.password);
    LOGD(buf_);

    is_mqtt_set = true;
    EEPROM.put(MQTT_HOST_EEPROM_POS, mqtt_config_to_save.host);
    EEPROM.put(MQTT_PORT_EEPROM_POS, mqtt_config_to_save.port);
    EEPROM.put(MQTT_USER_EEPROM_POS, mqtt_config_to_save.user);
    EEPROM.put(MQTT_PASS_EEPROM_POS, mqtt_config_to_save.password);
    EEPROM.put(MQTT_SET_EEPROM_POS, is_mqtt_set);

    return EEPROM.commit();
}

MQTTConfiguration Configuration::getMQTTConfiguration()
{
    return mqtt_config;
}

bool Configuration::loadMQTTConfiguration()
{
    char buf_[512];

    EEPROM.get(MQTT_HOST_EEPROM_POS, mqtt_config.host);
    EEPROM.get(MQTT_PORT_EEPROM_POS, mqtt_config.port);
    EEPROM.get(MQTT_USER_EEPROM_POS, mqtt_config.user);
    EEPROM.get(MQTT_PASS_EEPROM_POS, mqtt_config.password);
    EEPROM.get(MQTT_SET_EEPROM_POS, is_mqtt_set);

    sprintf(buf_, "loaded MQTT credentials %s %d %s %s %d", mqtt_config.host, mqtt_config.port, mqtt_config.user, mqtt_config.password, is_mqtt_set);
    LOGD(buf_);

    return is_mqtt_set;
}

bool Configuration::saveOSConfigurationInMemory(OSConfiguration os_config)
{

    this->os_config.mode = os_config.mode;
    char buf_[32];
    sprintf(buf_, "os mode set to %d", os_config.mode);
    LOGD(buf_);

    return true;
}

bool Configuration::saveOSConfiguration(OSConfiguration os_config)
{
    {
        SemaphoreGuard lock(mutex_);
        EEPROM.put(OS_MODE_EEPROM_POS, os_config.mode);
    }

    return EEPROM.commit();
}

bool Configuration::loadOSConfiguration()
{
    // boot mode
    EEPROM.get(OS_MODE_EEPROM_POS, os_config.mode);

    if (os_config.mode > OSMode::HASS)
    {
        os_config.mode = OSMode::ONBOARDING;
    }

    if (os_config.mode < 0)
    {
        os_config.mode = OSMode::ONBOARDING;
    }

    return true;
}

bool Configuration::saveFactoryStrainCalibration(float strain_scale)
{
    {
        SemaphoreGuard lock(mutex_);
        pb_buffer_.strain_scale = strain_scale;
    }
    return saveToDisk();
}

OSConfiguration *Configuration::getOSConfiguration()
{
    return &os_config;
}

PB_PersistentConfiguration Configuration::get()
{
    SemaphoreGuard lock(mutex_);
    if (!loaded_)
    {
        return PB_PersistentConfiguration();
    }
    return pb_buffer_;
}

bool Configuration::setMotorCalibrationAndSave(PB_MotorCalibration &motor_calibration)
{
    {
        SemaphoreGuard lock(mutex_);
        pb_buffer_.motor = motor_calibration;
        pb_buffer_.has_motor = true;
    }
    return saveToDisk();
}

void Configuration::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void Configuration::publishEvent(WiFiEvent event)
{
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}