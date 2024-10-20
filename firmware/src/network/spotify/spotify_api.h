#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <logging.h>

#include "base64.h"
#include "proto/proto_gen/smartknob.pb.h"
#include "./structs.h"
#include "./events/events.h"

// const char *client_id = "your_spotify_client_id";         // Your Spotify client ID
// const char *client_secret = "your_spotify_client_secret"; // Your Spotify client secret
// const char *redirect_uri = "your_redirect_uri";           // Your redirect URI from the Spotify dashboard

class SpotifyApi
{
public:
    SpotifyApi(PB_SpotifyConfig spotify_config) : spotify_config_(spotify_config) {};
    ~SpotifyApi() {};

    bool play();
    bool pause();

    void setConfig(const PB_SpotifyConfig &spotify_config);
    PB_SpotifyConfig getConfig();

    bool isAvailable();
    bool isPlaying();

    String getCurrentTrackName();

    PlaybackState getCurrentPlaybackState();

    uint8_t *imageBuffer = nullptr;
    // const char *imageUrl = "https://i.scdn.co/image/ab67616d000048515a2ed586c163472d6d3f8532";

    size_t imageSize = 0;

    void downloadImage();

    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

private:
    // String redirect_uri = "your_redirect_uri";
    // String scope = "user-read-playback-state"; // Example scope
    // char code_challenge[128];                  // Pre-generated code challenge
    // char code_verifier[128];                   // Pre-generated code verifier

    // char client_id_[512] = "your_spotify_client_id";
    // char client_secret_[512] = "your_spotify_client_secret";
    unsigned long last_refreshed_ms_ = 0;
    unsigned long last_fetched_state_ms_ = 0;
    PlaybackState latest_playback_state_{.timestamp = 0};

    PB_SpotifyConfig spotify_config_;

    bool checkAndRefreshToken();
    void refreshToken();

    int16_t sendPutRequest(const String &url, const String &body = "");

    QueueHandle_t shared_events_queue;
    void publishEvent(WiFiEvent event);
};