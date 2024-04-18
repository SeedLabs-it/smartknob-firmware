#pragma once

#include "logger.h"

#define LOG(log_level, ...)                                                                  \
    do                                                                                       \
    {                                                                                        \
        if (Logger *logger = Logging::getInstance().getLogger())                             \
        {                                                                                    \
            char origin_[256];                                                               \
            char msg_[256];                                                                  \
            snprintf(origin_, sizeof(origin_), "[%s:%s:%d] ", __FILE__, __func__, __LINE__); \
            snprintf(msg_, sizeof(msg_), ##__VA_ARGS__);                                     \
            logger->log(log_level, origin_, msg_);                                           \
        }                                                                                    \
    } while (0)

#define LOGI(...)                           \
    do                                      \
    {                                       \
        LOG(PB_LogLevel_INFO, __VA_ARGS__); \
    } while (0)

#define LOGW(...)                              \
    do                                         \
    {                                          \
        LOG(PB_LogLevel_WARNING, __VA_ARGS__); \
    } while (0)

#define LOGE(...)                            \
    do                                       \
    {                                        \
        LOG(PB_LogLevel_ERROR, __VA_ARGS__); \
    } while (0)

#define LOGD(...)                            \
    do                                       \
    {                                        \
        LOG(PB_LogLevel_DEBUG, __VA_ARGS__); \
    } while (0)

class Logging
{

public:
    static Logging &getInstance()
    {
        static Logging instance;
        return instance;
    }

    void setLogger(Logger *logger)
    {
        logger_ = logger;
    }

    Logger *getLogger()
    {
        return logger_;
    }

private:
    Logging(){};
    ~Logging(){};

    Logging *instance_ = nullptr;
    Logger *logger_ = nullptr;
};