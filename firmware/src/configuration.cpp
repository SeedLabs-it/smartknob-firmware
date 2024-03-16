#include <FFat.h>

#include "pb_decode.h"
#include "pb_encode.h"

#include "proto_gen/smartknob.pb.h"
#include "semaphore_guard.h"

#include "configuration.h"

static const char *CONFIG_PATH = "/config.pb";

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
    FatGuard fatGuard(logger_);
    if (!fatGuard.mounted_)
    {
        return false;
    }

    File f = FFat.open(CONFIG_PATH);
    if (!f)
    {
        log("Failed to read config file");
        ESP_LOGD("", "Failed to read config file");
        return false;
    }

    size_t read = f.readBytes((char *)buffer_, sizeof(buffer_));
    f.close();

    pb_istream_t stream = pb_istream_from_buffer(buffer_, read);
    if (!pb_decode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_))
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Decoding failed: %s", PB_GET_ERROR(&stream));
        log(buf);
        pb_buffer_ = {};
        return false;
    }

    if (pb_buffer_.version != PERSISTENT_CONFIGURATION_VERSION)
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Invalid config version. Expected %u, received %u", PERSISTENT_CONFIGURATION_VERSION, pb_buffer_.version);
        log(buf);
        pb_buffer_ = {};
        return false;
    }
    loaded_ = true;

    char buf[200];
    snprintf(
        buf,
        sizeof(buf),
        "Motor calibration: calib=%u, pole_pairs=%u, zero_offset=%.2f, cw=%u",
        pb_buffer_.motor.calibrated,
        pb_buffer_.motor.pole_pairs,
        pb_buffer_.motor.zero_electrical_offset,
        pb_buffer_.motor.direction_cw);
    log(buf);
    return true;
}

bool Configuration::saveToDisk()
{
    SemaphoreGuard lock(mutex_);

    pb_ostream_t stream = pb_ostream_from_buffer(buffer_, sizeof(buffer_));
    pb_buffer_.version = PERSISTENT_CONFIGURATION_VERSION;
    if (!pb_encode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_))
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Encoding failed: %s", PB_GET_ERROR(&stream));
        log(buf);
        return false;
    }
    ESP_LOGD("1", "save to disk");

    FatGuard fatGuard(logger_);
    if (!fatGuard.mounted_)
    {
        return false;
    }
    ESP_LOGD("2", "save to disk");

    File f = FFat.open(CONFIG_PATH, FILE_WRITE);
    if (!f)
    {
        ESP_LOGD("2.5", "Failed to read config file");

        log("Failed to read config file");
        return false;
    }
    ESP_LOGD("3", "save to disk");

    size_t written = f.write(buffer_, stream.bytes_written);
    f.close();
    ESP_LOGD("4", "save to disk");

    char buf[20];
    snprintf(buf, sizeof(buf), "Wrote %d bytes", written);
    log(buf);
    ESP_LOGD("5", "save to disk");

    if (written != stream.bytes_written)
    {
        log("Failed to write all bytes to file");
        return false;
    }

    return true;
}

bool Configuration::saveWiFiConfiguration(WiFiConfiguration wifi_config_to_save)
{
    // TODO: persist in a file
    char buf_[512];
    sprintf(buf_, "saving wifi credentials %s %s", wifi_config_to_save.ssid, wifi_config_to_save.passphrase);
    log(buf_);

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
    log(buf_);

    return is_wifi_set;
}

bool Configuration::saveMQTTConfiguration(MQTTConfiguration mqtt_config_to_save)
{
    // TODO: persist in a file
    char buf_[512];
    sprintf(buf_, "saving MQTT credentials %s %d %s %s", mqtt_config_to_save.host, mqtt_config_to_save.port, mqtt_config_to_save.user, mqtt_config_to_save.password);
    log(buf_);

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
    log(buf_);

    return is_mqtt_set;
}

bool Configuration::saveOSConfigurationInMemory(OSConfiguration os_config)
{

    this->os_config.mode = os_config.mode;
    char buf_[32];
    sprintf(buf_, "os mode set to %d", os_config.mode);
    log(buf_);

    return true;
}

bool Configuration::saveOSConfiguration(OSConfiguration os_config)
{
    EEPROM.put(OS_MODE_EEPROM_POS, os_config.mode);

    return EEPROM.commit();
}

bool Configuration::loadOSConfiguration()
{
    // boot mode
    EEPROM.get(OS_MODE_EEPROM_POS, os_config.mode);

    if (os_config.mode > Hass)
    {
        os_config.mode = Onboarding;
    }

    if (os_config.mode < 0)
    {
        os_config.mode = Onboarding;
    }

    return true;
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

bool Configuration::setStrainCalibrationAndSave(PB_StrainCalibration &strain_calibration)
{
    {
        SemaphoreGuard lock(mutex_);
        pb_buffer_.strain = strain_calibration;
        pb_buffer_.has_strain = true;
    }
    return saveToDisk();
}

void Configuration::setLogger(Logger *logger)
{
    logger_ = logger;
}

void Configuration::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}
