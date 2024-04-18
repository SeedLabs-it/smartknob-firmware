#pragma once
#include "proto_gen/smartknob.pb.h"
class Logger
{

public:
    Logger(){};
    virtual ~Logger(){};
    virtual void log(const char *msg) = 0;
    // virtual void logWarning(const char *msg) = 0;
    // virtual void logError(const char *msg) = 0;
    // virtual void logDebug(const char *msg) = 0;
    virtual void log(const PB_LogLevel log_level, const char *msg) = 0;
};