#pragma once
#include <Arduino.h>

// #define MAX_MARKETS 5
#define MAX_IMAGES 10
// #define MAX_ARTISTS 5

struct Image
{
    char url[128];
    int height;
    int width;
};

struct Album
{
    Image images[MAX_IMAGES]; // Fixed-size array for images
    char name[64];
};

struct ExternalIds
{
    char isrc[32];
    char ean[32];
    char upc[32];
};

struct Item
{
    Album album;
    int duration_ms;
    char name[64];
};

struct Actions
{
    bool interrupting_playback;
    bool pausing;
    bool resuming;
    bool seeking;
    bool skipping_next;
    bool skipping_prev;
    bool toggling_repeat_context;
    bool toggling_shuffle;
    bool toggling_repeat_track;
    bool transferring_playback;
};

struct Device
{
    char id[64];
    bool is_active;
    bool is_private_session;
    bool is_restricted;
    char name[64];
    char type[32];
    int volume_percent;
    bool supports_volume;
};

struct PlaybackState
{
    bool available;
    Device device;
    char repeat_state[16];
    bool shuffle_state;
    uint64_t timestamp;
    int progress_ms;
    bool is_playing;
    Item item;
    char currently_playing_type[32];
    Actions actions;
};