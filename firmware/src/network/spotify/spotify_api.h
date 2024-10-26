#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <logging.h>

#include "base64.h"
#include "proto/proto_gen/smartknob.pb.h"
#include "./structs.h"
#include "./events/events.h"
#include "./configuration.h"

// const char *client_id = "your_spotify_client_id";         // Your Spotify client ID
// const char *client_secret = "your_spotify_client_secret"; // Your Spotify client secret
// const char *redirect_uri = "your_redirect_uri";           // Your redirect URI from the Spotify dashboard

#define AUTH_HEADER_SIZE 524
#define REFRESH_TOKEN_SIZE 524
#define BASE64_BASIC_AUTH_HEADER_SIZE 256

class SpotifyApi
{
public:
    // SpotifyApi(PB_SpotifyConfig spotify_config) : spotify_config_(spotify_config) {};
    SpotifyApi(Configuration &configuration);
    ~SpotifyApi();

    bool play(const char *device_id);
    bool pause(const char *device_id);

    // void setConfig(const PB_SpotifyConfig &spotify_config);
    // PB_SpotifyConfig getConfig();

    bool isAvailable();
    bool isPlaying();

    String getCurrentTrackName();

    PlaybackState getCurrentPlaybackState(bool override = false);

    uint8_t *imageBuffer = nullptr;
    // const char *imageUrl = "https://i.scdn.co/image/ab67616d000048515a2ed586c163472d6d3f8532";

    size_t imageSize = 0;

    void downloadImage();

    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

    void setConfig(const PB_SpotifyConfig &spotify_config);

    // void setAuthHeader(const char *access_token);
    // char *getAuthHeader();

    // void setRefreshTokenBody(const char *refresh_token);
    // void setExpiresIn(uint16_t expires_in);

private:
    // PB_SpotifyConfig *spotify_config_ = nullptr;
    HTTPClient http_client_;

    Configuration &configuration_;

    uint16_t expires_in_ = 0;

    // char auth_header_[524];
    // char refresh_token_body_[552];
    // char base64_basic_auth_header_[256];
    char *auth_header_ = nullptr;
    char *refresh_token_body_ = nullptr;
    char *base64_basic_auth_header_ = nullptr;
    // String redirect_uri = "your_redirect_uri";
    // String scope = "user-read-playback-state"; // Example scope
    // char code_challenge[128];                  // Pre-generated code challenge
    // char code_verifier[128];                   // Pre-generated code verifier

    // char client_id_[512] = "your_spotify_client_id";
    // char client_secret_[512] = "your_spotify_client_secret";
    unsigned long last_refreshed_ms_ = 0;

    // PB_SpotifyConfig spotify_config_;

    bool checkAndRefreshToken();
    bool refreshToken();

    int sendPutRequest(const String &url, const String &body = "");

    // QueueHandle_t shared_events_queue;
    // void publishEvent(WiFiEvent event);
};