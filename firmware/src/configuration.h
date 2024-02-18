#pragma once

#include <FFat.h>
#include <PacketSerial.h>

#include "proto_gen/smartknob.pb.h"

#include "logger.h"

#include "EEPROM.h"

// TODO: should move these consts to wifi?
static const uint16_t WIFI_SSID_LENGTH = 128;
static const uint16_t WIFI_PASSPHRASE_LENGTH = 128;
static const uint16_t WIFI_SET_LENGTH = 1;

// Some exta bytes for local domains
static const uint16_t MQTT_HOST_LENGTH = 64;
static const uint16_t MQTT_PORT_LENGTH = 4;
static const uint16_t MQTT_USER_LENGTH = 64;
static const uint16_t MQTT_PASS_LENGTH = 64;
static const uint16_t MQTT_SET_LENGTH = 1;

// OS configurations
static const uint16_t OS_MODE_LENGTH = 1;
static const uint16_t OS_CONFIG_TOTAL_LENGTH = 50;

// OS config EEPROM positions
static const uint16_t OS_MODE_EEPROM_POS = 0;

// WiFi EEPROM positions
static const uint16_t WIFI_SSID_EEPROM_POS = OS_MODE_EEPROM_POS + OS_CONFIG_TOTAL_LENGTH;
static const uint16_t WIFI_PASSPHRASE_EEPROM_POS = WIFI_SSID_LENGTH;
static const uint16_t WIFI_SET_EEPROM_POS = WIFI_PASSPHRASE_EEPROM_POS + WIFI_PASSPHRASE_LENGTH;

// MQTT EEPROM positions
static const uint16_t MQTT_HOST_EEPROM_POS = WIFI_SET_EEPROM_POS + WIFI_SET_LENGTH;
static const uint16_t MQTT_PORT_EEPROM_POS = MQTT_HOST_EEPROM_POS + MQTT_HOST_LENGTH;
static const uint16_t MQTT_USER_EEPROM_POS = MQTT_PORT_EEPROM_POS + MQTT_PORT_LENGTH;
static const uint16_t MQTT_PASS_EEPROM_POS = MQTT_USER_EEPROM_POS + MQTT_USER_LENGTH;
static const uint16_t MQTT_SET_EEPROM_POS = MQTT_PASS_EEPROM_POS + MQTT_PASS_LENGTH;

// EEPROM size, verify when adding new fiels that size is still big enough
static const uint16_t EEPROM_SIZE = 512;

const uint32_t PERSISTENT_CONFIGURATION_VERSION = 1;

struct WiFiConfiguration
{
    char ssid[128];
    char passphrase[128];
};

enum OSMode
{
    Onboarding = 0,
    Demo = 1,
    Hass = 2
};

struct OSConfiguration
{
    OSMode mode = Onboarding;
};

class Configuration
{
public:
    Configuration();
    ~Configuration();

    void setLogger(Logger *logger);
    bool loadFromDisk();
    bool saveToDisk();
    PB_PersistentConfiguration get();
    bool setMotorCalibrationAndSave(PB_MotorCalibration &motor_calibration);
    bool setStrainCalibrationAndSave(PB_StrainCalibration &strain_calibration);
    bool saveWiFiConfiguration(WiFiConfiguration wifi_config);
    WiFiConfiguration getWiFiConfiguration();
    bool loadWiFiConfiguration();
    bool saveOSConfiguration(OSConfiguration os_config);
    bool saveOSConfigurationInMemory(OSConfiguration os_config);
    bool loadOSConfiguration();
    OSConfiguration *getOSConfiguration();

private:
    SemaphoreHandle_t mutex_;

    Logger *logger_ = nullptr;
    bool loaded_ = false;
    PB_PersistentConfiguration pb_buffer_ = {};

    WiFiConfiguration wifi_config;
    bool is_wifi_set = false;
    OSConfiguration os_config;

    uint8_t buffer_[PB_PersistentConfiguration_size];

    void log(const char *msg);
};
class FatGuard
{
public:
    FatGuard(Logger *logger) : logger_(logger)
    {
        if (!FFat.begin(true))
        {
            if (logger_ != nullptr)
            {
                logger_->log("Failed to mount FFat");
            }
            return;
        }
        if (logger_ != nullptr)
        {
            logger_->log("Mounted FFat");
        }
        mounted_ = true;
    }
    ~FatGuard()
    {
        if (mounted_)
        {
            FFat.end();
            if (logger_ != nullptr)
            {
                logger_->log("Unmounted FFat");
            }
        }
    }
    FatGuard(FatGuard const &) = delete;
    FatGuard &operator=(FatGuard const &) = delete;

    bool mounted_ = false;

private:
    Logger *logger_;
};
