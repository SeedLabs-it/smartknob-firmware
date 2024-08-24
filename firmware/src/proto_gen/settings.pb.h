/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_SETTINGS_SETTINGS_PB_H_INCLUDED
#define PB_SETTINGS_SETTINGS_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _SETTINGS_Screen {
    bool dim;
    int32_t max_bright;
    int32_t min_bright;
    int32_t timeout;
} SETTINGS_Screen;

typedef struct _SETTINGS_Beacon {
    bool enabled;
    int32_t brightness;
    int32_t color;
} SETTINGS_Beacon;

typedef struct _SETTINGS_LedRing {
    bool enabled;
    bool dim;
    int32_t max_bright;
    int32_t min_bright;
    int32_t timeout;
    int32_t color;
    bool has_beacon;
    SETTINGS_Beacon beacon;
} SETTINGS_LedRing;

typedef struct _SETTINGS_Settings {
    uint8_t protocol_version;
    bool has_screen;
    SETTINGS_Screen screen;
    bool has_led_ring;
    SETTINGS_LedRing led_ring;
} SETTINGS_Settings;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define SETTINGS_Settings_init_default           {0, false, SETTINGS_Screen_init_default, false, SETTINGS_LedRing_init_default}
#define SETTINGS_Screen_init_default             {0, 0, 0, 0}
#define SETTINGS_Beacon_init_default             {0, 0, 0}
#define SETTINGS_LedRing_init_default            {0, 0, 0, 0, 0, 0, false, SETTINGS_Beacon_init_default}
#define SETTINGS_Settings_init_zero              {0, false, SETTINGS_Screen_init_zero, false, SETTINGS_LedRing_init_zero}
#define SETTINGS_Screen_init_zero                {0, 0, 0, 0}
#define SETTINGS_Beacon_init_zero                {0, 0, 0}
#define SETTINGS_LedRing_init_zero               {0, 0, 0, 0, 0, 0, false, SETTINGS_Beacon_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define SETTINGS_Screen_dim_tag                  1
#define SETTINGS_Screen_max_bright_tag           2
#define SETTINGS_Screen_min_bright_tag           3
#define SETTINGS_Screen_timeout_tag              4
#define SETTINGS_Beacon_enabled_tag              1
#define SETTINGS_Beacon_brightness_tag           2
#define SETTINGS_Beacon_color_tag                3
#define SETTINGS_LedRing_enabled_tag             1
#define SETTINGS_LedRing_dim_tag                 2
#define SETTINGS_LedRing_max_bright_tag          3
#define SETTINGS_LedRing_min_bright_tag          4
#define SETTINGS_LedRing_timeout_tag             5
#define SETTINGS_LedRing_color_tag               6
#define SETTINGS_LedRing_beacon_tag              7
#define SETTINGS_Settings_protocol_version_tag   1
#define SETTINGS_Settings_screen_tag             2
#define SETTINGS_Settings_led_ring_tag           3

/* Struct field encoding specification for nanopb */
#define SETTINGS_Settings_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   protocol_version,   1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  screen,            2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  led_ring,          3)
#define SETTINGS_Settings_CALLBACK NULL
#define SETTINGS_Settings_DEFAULT NULL
#define SETTINGS_Settings_screen_MSGTYPE SETTINGS_Screen
#define SETTINGS_Settings_led_ring_MSGTYPE SETTINGS_LedRing

#define SETTINGS_Screen_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     dim,               1) \
X(a, STATIC,   SINGULAR, INT32,    max_bright,        2) \
X(a, STATIC,   SINGULAR, INT32,    min_bright,        3) \
X(a, STATIC,   SINGULAR, INT32,    timeout,           4)
#define SETTINGS_Screen_CALLBACK NULL
#define SETTINGS_Screen_DEFAULT NULL

#define SETTINGS_Beacon_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     enabled,           1) \
X(a, STATIC,   SINGULAR, INT32,    brightness,        2) \
X(a, STATIC,   SINGULAR, INT32,    color,             3)
#define SETTINGS_Beacon_CALLBACK NULL
#define SETTINGS_Beacon_DEFAULT NULL

#define SETTINGS_LedRing_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     enabled,           1) \
X(a, STATIC,   SINGULAR, BOOL,     dim,               2) \
X(a, STATIC,   SINGULAR, INT32,    max_bright,        3) \
X(a, STATIC,   SINGULAR, INT32,    min_bright,        4) \
X(a, STATIC,   SINGULAR, INT32,    timeout,           5) \
X(a, STATIC,   SINGULAR, INT32,    color,             6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  beacon,            7)
#define SETTINGS_LedRing_CALLBACK NULL
#define SETTINGS_LedRing_DEFAULT NULL
#define SETTINGS_LedRing_beacon_MSGTYPE SETTINGS_Beacon

extern const pb_msgdesc_t SETTINGS_Settings_msg;
extern const pb_msgdesc_t SETTINGS_Screen_msg;
extern const pb_msgdesc_t SETTINGS_Beacon_msg;
extern const pb_msgdesc_t SETTINGS_LedRing_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define SETTINGS_Settings_fields &SETTINGS_Settings_msg
#define SETTINGS_Screen_fields &SETTINGS_Screen_msg
#define SETTINGS_Beacon_fields &SETTINGS_Beacon_msg
#define SETTINGS_LedRing_fields &SETTINGS_LedRing_msg

/* Maximum encoded size of messages (where known) */
#define SETTINGS_Beacon_size                     24
#define SETTINGS_LedRing_size                    74
#define SETTINGS_SETTINGS_PB_H_MAX_SIZE          SETTINGS_Settings_size
#define SETTINGS_Screen_size                     35
#define SETTINGS_Settings_size                   116

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
