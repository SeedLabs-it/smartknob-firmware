#pragma once

#include "logger.h"
#include "task.h"
#include "esp_ota_ops.h"
#include "configuration.h"

class ResetTask : public Task<ResetTask>
{
    friend class Task<ResetTask>; // Allow base Task to invoke protected run()

public:
    ResetTask(const uint8_t task_core, Configuration &configuration);
    ~ResetTask();

    void setLogger(Logger *logger);
    void setVerbose(bool verbose);
    void toggleVerbose();

protected:
    void run();

private:
    Configuration &configuration_;
    Logger *logger_;
    void log(const char *msg);
    bool verbose_ = false;
    char buf_[128];
};
