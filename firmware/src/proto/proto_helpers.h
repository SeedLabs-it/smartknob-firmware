#pragma once

#include "proto_gen/smartknob.pb.h"
#include <logging.h>

#define PROTOBUF_PROTOCOL_VERSION (1)

inline bool config_eq(PB_SmartKnobConfig &first, PB_SmartKnobConfig &second)
{
    return first.detent_strength_unit == second.detent_strength_unit &&
           first.endstop_strength_unit == second.endstop_strength_unit &&
           first.position == second.position &&
           first.position_nonce == second.position_nonce &&
           first.min_position == second.min_position &&
           first.max_position == second.max_position &&
           first.position_width_radians == second.position_width_radians &&
           first.snap_point == second.snap_point &&
           first.sub_position_unit == second.sub_position_unit &&
           strcmp(first.id, second.id) == 0 &&
           first.detent_positions_count == second.detent_positions_count &&
           memcmp(first.detent_positions, second.detent_positions,
                  first.detent_positions_count *
                      sizeof(first.detent_positions[0])) &&
           first.snap_point_bias == second.snap_point_bias;
}

inline bool state_eq(PB_SmartKnobState &first, PB_SmartKnobState &second)
{
    return first.has_config == second.has_config &&
           (!first.has_config || config_eq(first.config, second.config)) &&
           first.current_position == second.current_position &&
           first.sub_position_unit == second.sub_position_unit;
}

class LogLevelConverter
{
public:
    // Conversion from LOG_LEVEL to PB_LogLevel
    static PB_LogLevel toPBLogLevel(LOG_LEVEL logLevel)
    {
        switch (logLevel)
        {
        case LOG_LEVEL_INFO:
            return PB_LogLevel_INFO;
        case LOG_LEVEL_WARNING:
            return PB_LogLevel_WARNING;
        case LOG_LEVEL_ERROR:
            return PB_LogLevel_ERROR;
        case LOG_LEVEL_DEBUG:
            return PB_LogLevel_DEBUG;
        default:
            return PB_LogLevel_VERBOSE; // Or handle as needed
        }
    }

    // Conversion from PB_LogLevel to LOG_LEVEL
    static LOG_LEVEL toLogLevel(PB_LogLevel pbLogLevel)
    {
        switch (pbLogLevel)
        {
        case PB_LogLevel_INFO:
            return LOG_LEVEL_INFO;
        case PB_LogLevel_WARNING:
            return LOG_LEVEL_WARNING;
        case PB_LogLevel_ERROR:
            return LOG_LEVEL_ERROR;
        case PB_LogLevel_DEBUG:
            return LOG_LEVEL_DEBUG;
        default:
            return LOG_LEVEL_COUNT; // Or handle as needed
        }
    }
};

inline PB_LogLevel operator+(LOG_LEVEL logLevel, LogLevelConverter)
{
    return LogLevelConverter::toPBLogLevel(logLevel);
}

inline LOG_LEVEL operator+(PB_LogLevel pbLogLevel, LogLevelConverter)
{
    return LogLevelConverter::toLogLevel(pbLogLevel);
}