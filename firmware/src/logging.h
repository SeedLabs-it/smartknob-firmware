#pragma once

#include "logger.h"

// ! CHAR[] CAUASES SOME PROBLEMS WITH MEMORY HERE IF ALOT OF LOG MESSAGES IN ONE TASK..... NEED BETTER SOLUTION
#define LOG(log_level, isVerbose_, ...)                                                   \
    do                                                                                    \
    {                                                                                     \
        if (Logger *logger = Logging::getInstance().getLogger())                          \
        {                                                                                 \
            if (isVerbose_ && !logger->isVerbose())                                       \
            {                                                                             \
                break;                                                                    \
            }                                                                             \
            char origin_[128];                                                            \
            char msg_[256];                                                               \
            snprintf(origin_, sizeof(origin_), "%s:%s:%d", __FILE__, __func__, __LINE__); \
            snprintf(msg_, sizeof(msg_), __VA_ARGS__);                                    \
            logger->log(log_level, isVerbose_, origin_, msg_);                            \
        }                                                                                 \
    } while (0)

#define LOGI(...)                                  \
    do                                             \
    {                                              \
        LOG(PB_LogLevel_INFO, false, __VA_ARGS__); \
    } while (0)

#define LOGW(...)                                     \
    do                                                \
    {                                                 \
        LOG(PB_LogLevel_WARNING, false, __VA_ARGS__); \
    } while (0)

#define LOGE(...)                                   \
    do                                              \
    {                                               \
        LOG(PB_LogLevel_ERROR, false, __VA_ARGS__); \
    } while (0)

#define LOGD(...)                                   \
    do                                              \
    {                                               \
        LOG(PB_LogLevel_DEBUG, false, __VA_ARGS__); \
    } while (0)

#define LOGV(log_level, ...)                 \
    do                                       \
    {                                        \
        LOG(log_level, true, ##__VA_ARGS__); \
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