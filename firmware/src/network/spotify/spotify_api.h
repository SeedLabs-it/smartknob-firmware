#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <logging.h>
#include <cstdlib>
#include <AsyncTCP.h>

#include "base64.h"
#include "proto/proto_gen/smartknob.pb.h"
#include "./structs.h"
#include "./events/events.h"
#include "./configuration.h"
#include "./util.h"

#include "./network/async_http.h"

#define AUTH_HEADER_SIZE 524
#define REFRESH_TOKEN_SIZE 524
#define BASE64_BASIC_AUTH_HEADER_SIZE 256

class SpotifyApi
{
public:
    SpotifyApi(Configuration &configuration);
    ~SpotifyApi();

    bool play(const char *device_id);
    bool pause(const char *device_id);

    bool setVolume(uint8_t volume, const char *device_id);

    // void setConfig(const PB_SpotifyConfig &spotify_config);
    // PB_SpotifyConfig getConfig();

    bool isAvailable();
    bool isPlaying();

    String getCurrentTrackName();

    PlaybackState getCurrentPlaybackState();

    uint8_t *imageBuffer = nullptr;
    size_t imageSize = 0;

    lv_color_t *colors = nullptr;

    void downloadImage(char *path);
    void fetchImageColors(char *path);

    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

    void setConfig(const PB_SpotifyConfig &spotify_config);
    bool hasConfig() { return auth_header_ != nullptr; }

private:
    HTTPClient http_client_;
    HTTPClient http_client_image_;

    Configuration &configuration_;

    uint16_t expires_in_ = 0;

    char *auth_header_ = nullptr;
    char *refresh_token_body_ = nullptr;
    char *base64_basic_auth_header_ = nullptr;

    char device_id_[64] = ""; // Your device ID

    unsigned long last_refreshed_ms_ = 0;

    bool checkAndRefreshToken();
    bool refreshToken();

    int sendPutRequest(const String &url, const String &body = "");

    static void getImageCb(void *arg, AsyncClient *client, void *data, size_t len);
    static void getColorsCb(void *arg, AsyncClient *client, void *data, size_t len);

    QueueHandle_t shared_events_queue_;
    void publishEvent(WiFiEvent event);
};