#pragma once

#if SK_DISPLAY_LVGL
#include <Arduino.h>
#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"
#include <TFT_eSPI.h>
#include "smklvgl/smartknob_lvgl.h"
#include <lvgl.h>

#define SMK_DISP_HOR_RES 240
#define SMK_DISP_VER_RES 240

#include <stdint.h>

class LvglTask : public Task<LvglTask>
{
    friend class Task<LvglTask>; // Allow base Task to invoke protected run()

public:
    LvglTask(const uint8_t task_core);
    ~LvglTask();

    QueueHandle_t getKnobStateQueue();
    void setLogger(Logger *logger);


    const char * rollerMhzOpts = "118\n119\n120\n121\n122\n123\n124\n125\n126\n127\n128\n129\n130\n131\n132\n133\n134\n135\n136";
    const char * rollerdkhzOpts = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
    const char * rollercmkhzOpts = "00\n05\n10\n15\n25\n30\n35\n40\n50\n55\n60\n65\n70\n80\n85\n90";
    
    /* data */
    uint32_t highlightTextColor = 0xffae1a;
    uint32_t unactiveTextColot = 0xdbdbdb;

    lv_style_t activeRollerStyle;
    lv_style_t inactiveRollerStyle;

    void demoLvgl();

protected:
    static lv_display_t *display_;

    static void smk_disp_flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
    static void lvgl_log_print(lv_log_level_t level, const char *file);
    void run();

private:
    QueueHandle_t app_state_queue_;
    static TFT_eSPI tft_; /* TFT instance */
    AppState app_state_;
    SemaphoreHandle_t mutex_;
    Logger *logger_;
    void log(const char *msg);

    OSMode os_mode;
};

#else

class LvglTask
{
};

#endif