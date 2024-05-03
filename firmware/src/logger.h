#pragma once
#include "proto_gen/smartknob.pb.h"

class Logger
{
public:
    Logger(){};
    virtual ~Logger(){};
    virtual void log(const char *msg) = 0;
    virtual void log(const PB_LogLevel log_level, bool isVerbose_, const char *origin, const char *msg) = 0;

    bool isVerbose()
    {
        return verbose;
    }
    void toggleVerbose()
    {
        verbose = !verbose;
    }

    bool logOrigin()
    {
        return log_origin;
    }
    void toggleLogOrigin()
    {
        log_origin = !log_origin;
    }

private:
    bool verbose = false;
    bool log_origin = false;
};
