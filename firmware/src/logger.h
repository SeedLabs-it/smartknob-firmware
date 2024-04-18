#pragma once
#include "proto_gen/smartknob.pb.h"

class Logger
{
public:
    Logger(){};
    virtual ~Logger(){};
    virtual void log(const char *msg) = 0;
    virtual void log(const PB_LogLevel log_level, const char *origin, const char *msg) = 0;

    bool isVerbose()
    {
        return verbose;
    }
    void toggleVerbose()
    {
        verbose = !verbose;
    }

private:
    bool verbose = false;
};
