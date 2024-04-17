#pragma once

enum LogLevel
{
    INFO,
    WARNING,
    ERROR
};
class Logger
{

public:
    Logger(){};
    virtual ~Logger(){};
    virtual void log(const char *msg) = 0;
    virtual void log(const LogLevel *log_level, const char *msg) = 0;
};