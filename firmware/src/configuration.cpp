#include <FFat.h>

#include "pb_decode.h"
#include "pb_encode.h"

#include "proto/proto_gen/smartknob.pb.h"
#include "semaphore_guard.h"

#include "configuration.h"

Configuration::Configuration()
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    wifi_config = WiFiConfiguration();
    mqtt_config = MQTTConfiguration();

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

pb_istream_t Configuration::loadFromDisk(const char *path, uint8_t *buffer, size_t buffer_size)
{
    FatGuard fatGuard;
    if (!fatGuard.mounted_)
    {
        return pb_istream_t{}; // TODO Look over this.
    }

    File f = FFat.open(path);
    if (!f)
    {
        LOGV(LOG_LEVEL_WARNING, "Load From Disk: Failed to read file, %s", path);
        return pb_istream_t{};
    }

    size_t read = f.readBytes((char *)buffer, buffer_size);
    f.close();

    return pb_istream_from_buffer(buffer, read);
}

bool Configuration::saveToDisk(const char *path, uint8_t *buffer, size_t buffer_size)
{
    FatGuard fatGuard;
    if (!fatGuard.mounted_)
    {
        return false;
    }

    File f = FFat.open(path, FILE_WRITE);
    if (!f)
    {
        LOGV(LOG_LEVEL_WARNING, "Failed to read file at, %s", path);
        return false;
    }

    size_t written = f.write(buffer, buffer_size);
    f.close();

    if (written != buffer_size)
    {
        LOGE("Failed to write all bytes to file, %s", path);
        return false;
    }

    return true;
}

bool Configuration::loadSpotifyConfigFromDisk()
{
    SemaphoreGuard lock(mutex_);

    pb_istream_t stream = loadFromDisk(SPOTIFY_CONFIG_PATH, spotify_config_stream_buffer_, sizeof(spotify_config_stream_buffer_));
    if (!pb_decode(&stream, PB_SpotifyConfig_fields, &spotify_config_buffer_))
    {
        LOGE("Decoding spotify config failed: %s", PB_GET_ERROR(&stream));
        spotify_config_buffer_ = {};
        return false;
    }

    if (spotify_config_buffer_.version != SPOTIFY_CONFIG_VERSION)
    {
        LOGE("Invalid config version for spotify config. Expected %u, received %u", SPOTIFY_CONFIG_VERSION, spotify_config_buffer_.version);
        spotify_config_buffer_ = {};
        return false;
    }
    spotify_config_loaded_ = true;

    return true;
}

bool Configuration::saveSpotifyConfigToDisk()
{
    {
        SemaphoreGuard lock(mutex_);

        pb_ostream_t stream = pb_ostream_from_buffer(spotify_config_stream_buffer_, sizeof(spotify_config_stream_buffer_));
        spotify_config_buffer_.version = SPOTIFY_CONFIG_VERSION;
        if (!pb_encode(&stream, PB_SpotifyConfig_fields, &spotify_config_buffer_))
        {
            char buf_[200];
            snprintf(buf_, sizeof(buf_), "Encoding failed: %s", PB_GET_ERROR(&stream));
            LOGE(buf_);
            return false;
        }

        saveToDisk(SPOTIFY_CONFIG_PATH, spotify_config_stream_buffer_, stream.bytes_written);
    }

    return true;
}

bool Configuration::setSpotifyConfig(const PB_SpotifyConfig &spotify_config)
{
    {
        SemaphoreGuard lock(mutex_);
        LOGV(LOG_LEVEL_DEBUG, "Setting Spotify config.");
        if (strcmp(spotify_config.base64_id_and_secret, "") != 0 && spotify_config_buffer_.base64_id_and_secret != spotify_config.base64_id_and_secret)
        {
            strcpy(spotify_config_buffer_.base64_id_and_secret, spotify_config.base64_id_and_secret);
        }
        if (strcmp(spotify_config.access_token, "") != 0 && spotify_config_buffer_.access_token != spotify_config.access_token)
        {
            strcpy(spotify_config_buffer_.access_token, spotify_config.access_token);
        }
        if (strcmp(spotify_config.token_type, "") != 0 && spotify_config_buffer_.token_type != spotify_config.token_type)
        {
            strcpy(spotify_config_buffer_.token_type, spotify_config.token_type);
        }
        if (strcmp(spotify_config.scope, "") != 0 && spotify_config_buffer_.scope != spotify_config.scope)
        {
            strcpy(spotify_config_buffer_.scope, spotify_config.scope);
        }
        if (spotify_config.expires_in != 0 && spotify_config_buffer_.expires_in != spotify_config.expires_in)
        {
            spotify_config_buffer_.expires_in = spotify_config.expires_in;
        }
        if (strcmp(spotify_config.refresh_token, "") != 0 && spotify_config_buffer_.refresh_token != spotify_config.refresh_token)
        {
            strcpy(spotify_config_buffer_.refresh_token, spotify_config.refresh_token);
        }
    }

    if (saveSpotifyConfigToDisk())
    {

        publishEvent(WiFiEvent{.type = SK_SPOTIFY_CONFIG_CHANGED});
        return true;
    }

    return false;
}

PB_SpotifyConfig Configuration::getSpotifyConfig()
{
    SemaphoreGuard lock(mutex_);
    if (!spotify_config_loaded_)
    {
        return PB_SpotifyConfig();
    }
    return spotify_config_buffer_;
}

bool Configuration::loadPersistantConfigFromDisk()
{
    SemaphoreGuard lock(mutex_);

    pb_istream_t stream = loadFromDisk(CONFIG_PATH, pb_stream_buffer_, sizeof(pb_stream_buffer_));
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
        LOGE("Invalid config version. Expected %u, received %u", PERSISTENT_CONFIGURATION_VERSION, pb_buffer_.version);
        pb_buffer_ = {};
        return false;
    }
    loaded_ = true;

    LOGV(LOG_LEVEL_DEBUG, "Motor calibration: calib=%u, pole_pairs=%u, zero_offset=%.2f, cw=%u",
         pb_buffer_.motor.calibrated,
         pb_buffer_.motor.pole_pairs,
         pb_buffer_.motor.zero_electrical_offset,
         pb_buffer_.motor.direction_cw);

    return true;
}

bool Configuration::savePersistantConfigToDisk()
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

        saveToDisk(CONFIG_PATH, pb_stream_buffer_, stream.bytes_written);
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

    pb_istream_t stream = loadFromDisk(SETTINGS_PATH, settings_stream_buffer_, sizeof(settings_stream_buffer_));
    if (!pb_decode(&stream, SETTINGS_Settings_fields, &settings_buffer_))
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Decoding settings failed: %s", PB_GET_ERROR(&stream));
        LOGE(buf_);
        settings_buffer_ = default_settings;
        return false;
    }

    if (settings_buffer_.protocol_version != SETTINGS_VERSION)
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Invalid config version. Expected %u, received %u", SETTINGS_VERSION, settings_buffer_.protocol_version);
        LOGE(buf_);
        settings_buffer_ = default_settings;
        return false;
    }
    settings_loaded_ = true;

    return true;
}

bool Configuration::saveSettingsToDisk()
{
    SemaphoreGuard lock(mutex_);

    pb_ostream_t stream = pb_ostream_from_buffer(settings_stream_buffer_, sizeof(settings_stream_buffer_));
    settings_buffer_.protocol_version = SETTINGS_VERSION;
    LOGE("Saving settings to disk");
    LOGE("COLOR %d", settings_buffer_.led_ring.color);
    if (!pb_encode(&stream, SETTINGS_Settings_fields, &settings_buffer_))
    {
        char buf_[200];
        snprintf(buf_, sizeof(buf_), "Encoding failed: %s", PB_GET_ERROR(&stream));
        LOGE(buf_);
        return false;
    }

    return saveToDisk(SETTINGS_PATH, settings_stream_buffer_, stream.bytes_written);
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
}

SETTINGS_Settings Configuration::getSettings()
{
    if (!settings_loaded_)
    {
        if (!loadSettingsFromDisk())
        {
            SemaphoreGuard lock(mutex_);
            LOGD("Settings couldnt load from disk, loading default settings instead.");
            settings_buffer_ = default_settings;
            settings_loaded_ = true;
            return settings_buffer_;
        }
    }
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
// TODO Move storage of config to fatfs instead of EEPROM
bool Configuration::loadWiFiConfiguration()
{
    EEPROM.get(WIFI_SSID_EEPROM_POS, wifi_config.ssid);
    EEPROM.get(WIFI_PASSPHRASE_EEPROM_POS, wifi_config.passphrase);
    EEPROM.get(WIFI_SET_EEPROM_POS, is_wifi_set);

    LOGV(LOG_LEVEL_DEBUG, "Loaded wifi credentials %s %s %d", wifi_config.ssid, wifi_config.passphrase, is_wifi_set);

    return is_wifi_set;
}

bool Configuration::saveMQTTConfiguration(MQTTConfiguration mqtt_config_to_save)
{
    LOGV(LOG_LEVEL_DEBUG, "Saving MQTT credentials %s %d %s %s", mqtt_config_to_save.host, mqtt_config_to_save.port, mqtt_config_to_save.user, mqtt_config_to_save.password);

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
    EEPROM.get(MQTT_HOST_EEPROM_POS, mqtt_config.host);
    EEPROM.get(MQTT_PORT_EEPROM_POS, mqtt_config.port);
    EEPROM.get(MQTT_USER_EEPROM_POS, mqtt_config.user);
    EEPROM.get(MQTT_PASS_EEPROM_POS, mqtt_config.password);
    EEPROM.get(MQTT_SET_EEPROM_POS, is_mqtt_set);

    LOGV(LOG_LEVEL_DEBUG, "Loaded MQTT credentials %s %d %s %s %d", mqtt_config.host, mqtt_config.port, mqtt_config.user, mqtt_config.password, is_mqtt_set);

    return is_mqtt_set;
}

bool Configuration::saveOSConfigurationInMemory(OSConfiguration os_config)
{

    this->os_config.mode = os_config.mode;
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
    return savePersistantConfigToDisk();
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
    return savePersistantConfigToDisk();
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