#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <logging.h>

#include "base64.h"
#include "proto/proto_gen/smartknob.pb.h"
#include "./structs.h"

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

    void setConfig(PB_SpotifyConfig spotify_config);

    bool isAvailable();
    bool isPlaying();

    PlaybackState getCurrentPlaybackState();

    uint8_t *imageBuffer = nullptr;
    // const char *imageUrl = "https://i.scdn.co/image/ab67616d000048515a2ed586c163472d6d3f8532";

    size_t imageSize = 0;

    void downloadImage()
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            char imageUrl[128] = "";
            PlaybackState playback_state = getCurrentPlaybackState();
            if (!playback_state.available)
            {
                LOGE("Playback state not available");
                return;
            }

            for (int i = 0; i < MAX_IMAGES; i++)
            {
                LOGE("Image %d: %s", i, playback_state.item.album.images[i].url);
                LOGE("Image %d: %d", i, playback_state.item.album.images[i].height);
                if (playback_state.item.album.images[i].height == 300)
                {
                    strcpy(imageUrl, playback_state.item.album.images[i].url);
                    break;
                }
            }

            if (strlen(imageUrl) == 0)
            {
                LOGE("No image found");
                return;
            }

            LOGE("Downloading image from: %s", imageUrl);

            HTTPClient http;
            http.begin(imageUrl);

            int httpCode = http.GET();
            if (httpCode == HTTP_CODE_OK)
            {
                imageSize = http.getSize();

                LOGE("Image size: %d", imageSize);

                // Allocate PSRAM for the image
                imageBuffer = (uint8_t *)heap_caps_aligned_alloc(16, imageSize, MALLOC_CAP_SPIRAM);

                LOGE("Allocated PSRAM for image.");
                // log buffer size
                LOGE("Buffer size: %d", sizeof(imageBuffer));

                if (imageBuffer != nullptr)
                {
                    WiFiClient *stream = http.getStreamPtr();
                    size_t offset = 0;
                    uint8_t buff[256];

                    // Read data in chunks and store it in PSRAM
                    // while (http.connected() && (stream->available() > 0) && (offset < imageSize))
                    // {
                    //     int bytesRead = stream->readBytes(buff, sizeof(buff));
                    //     memcpy(imageBuffer + offset, buff, bytesRead);
                    //     offset += bytesRead;
                    // }

                    while (http.connected() && (stream->available() > 0) && (offset < imageSize))
                    {
                        int bytesRead = stream->readBytes(buff, sizeof(buff));
                        if (bytesRead > 0)
                        {
                            memcpy(imageBuffer + offset, buff, bytesRead);
                            offset += bytesRead;
                        }
                    }

                    Serial.println("Image downloaded and stored in PSRAM.");
                }
                else
                {
                    Serial.println("Failed to allocate PSRAM.");
                }
            }
            else
            {
                LOGE("Error in HTTP request: %d", httpCode);
            }

            http.end();
        }
    }

private:
    // String redirect_uri = "your_redirect_uri";
    // String scope = "user-read-playback-state"; // Example scope
    // char code_challenge[128];                  // Pre-generated code challenge
    // char code_verifier[128];                   // Pre-generated code verifier

    // char client_id_[512] = "your_spotify_client_id";
    // char client_secret_[512] = "your_spotify_client_secret";
    unsigned long last_refreshed_ = 0;
    PlaybackState latest_playback_state_;

    PB_SpotifyConfig spotify_config_;

    bool checkAndRefreshToken();
    void refreshToken();

    bool sendPutRequest(const String &url, const String &body = "");
};