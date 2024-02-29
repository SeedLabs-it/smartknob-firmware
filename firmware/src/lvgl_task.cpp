#if SK_DISPLAY_LVGL

#include "lvgl_task.h"
#include <lvgl.h>

#define DRAW_BUF_SIZE (SMK_DISP_HOR_RES * SMK_DISP_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf_[DRAW_BUF_SIZE / 4];
TFT_eSPI LvglTask::tft_ = TFT_eSPI();
lv_display_t *LvglTask::display_ = nullptr;

static LvglTask *lvgl_task_p = nullptr;

LvglTask::LvglTask(const uint8_t task_core, LedRingTask *ledRingTask) : Task{"Lvgl", 1024 * 12, 1, task_core}
{
    app_state_queue_ = xQueueCreate(1, sizeof(AppState));
    assert(app_state_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    ledring_task_ = ledRingTask;
    lvgl_task_p = this;
}
LvglTask::~LvglTask()
{
    vQueueDelete(app_state_queue_);
    vSemaphoreDelete(mutex_);
}

QueueHandle_t LvglTask::getKnobStateQueue()
{
    return app_state_queue_;
}

void LvglTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void LvglTask::run()
{
    

    // HERE IS WHERE THE REAL LVGL IS STARTING
    
    //LvglTask::display_ = lv_tft_espi_create(SMK_DISP_HOR_RES, SMK_DISP_VER_RES, draw_buf_, DRAW_BUF_SIZE);
    //logger_->log("Display created");
    
#if LV_LOG_PRINTF == 0
    lv_log_register_print_cb(LvglTask::lvgl_log_print);
#endif
    logger_->log("LVGL initialized, initing TFT_eSPI...");
    LvglTask::tft_.begin();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // LvglTask::tft_.initDMA();
    logger_->log("TFT_eSPI initialized, setting up display...");
    LvglTask::tft_.invertDisplay(1);
    LvglTask::tft_.setRotation(0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    LvglTask::tft_.setRotation(0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_BLACK);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_RED);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_ORANGE);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_BLUE);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_PURPLE);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    LvglTask::tft_.fillScreen(TFT_BLACK);
    vTaskDelay(3000/portTICK_PERIOD_MS);
    logger_->log("Starting LVGL task");
    lv_init();
    logger_->log("Started");
    LvglTask::display_ = lv_display_create(SMK_DISP_HOR_RES, SMK_DISP_VER_RES);

    logger_->log("Display created, setting up display driver...");
    lv_display_set_driver_data(LvglTask::display_, (void *)&LvglTask::tft_);
    lv_display_set_flush_cb(LvglTask::display_, LvglTask::smk_disp_flush);
    lv_display_set_buffers(LvglTask::display_, (void *)draw_buf_, NULL, sizeof(draw_buf_), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(LvglTask::display_);
    
    lv_obj_t *display_obj = nullptr;

    
    display_obj = lv_screen_active();
    

    logger_->log("Display driver set, LVGL loop starting now...");
    while (1)
    {
        // uint32_t wait_ms = lv_timer_handler();
        // lv_tick_inc(wait_ms);
        lv_task_handler(); /* let the GUI do its work */
        delay(5);
    }
}

/* Display flushing */
void LvglTask::smk_disp_flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    LvglTask::tft_.startWrite();
    LvglTask::tft_.setAddrWindow(area->x1, area->y1, w, h);
    LvglTask::tft_.pushColors((uint16_t *)px_map, w * h, true);
    LvglTask::tft_.endWrite();

    lv_display_flush_ready(display);
}

/* Display log */
#if LV_LOG_PRINTF == 0
/* Serial debugging */
void LvglTask::lvgl_log_print(lv_log_level_t level, const char *file)
{
    lvgl_task_p->logger_->log(file);
}
#endif

void LvglTask::demoLvgl()
{
    //lv_obj_clean(lv_screen_active());
    lv_obj_set_scrollbar_mode(lv_screen_active(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *valueArc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(valueArc, 240, 240);
    lv_obj_set_style_arc_width(valueArc, 10, 0);
    lv_obj_set_style_arc_width(valueArc, 10, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(valueArc, lv_color_hex(0xDEDEDE), 0);
    lv_obj_set_style_arc_color(valueArc, lv_color_hex(highlightTextColor), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(valueArc, lv_color_hex(highlightTextColor), LV_PART_KNOB);
    lv_obj_set_style_pad_all(valueArc, 10, LV_PART_KNOB);
    lv_arc_set_rotation(valueArc, 180);
    lv_arc_set_bg_angles(valueArc, 0, 180);
    lv_obj_center(valueArc);

    lv_style_init(&activeRollerStyle);
    lv_style_set_text_font(&activeRollerStyle, lv_font_default());
    lv_style_set_text_color(&activeRollerStyle, lv_color_white());
    lv_style_set_bg_color(&activeRollerStyle, lv_color_hex(0xedae00));
    lv_style_set_border_width(&activeRollerStyle, 0);
    lv_style_set_border_color(&activeRollerStyle, lv_color_black());
    lv_style_set_pad_all(&activeRollerStyle, 3);
    lv_style_set_radius(&activeRollerStyle, 7);

    lv_style_init(&inactiveRollerStyle);
    lv_style_set_text_font(&inactiveRollerStyle, lv_font_default());
    lv_style_set_text_color(&inactiveRollerStyle, lv_color_white());
    lv_style_set_bg_color(&inactiveRollerStyle, lv_color_black());
    lv_style_set_border_width(&inactiveRollerStyle, 0);
    lv_style_set_border_color(&activeRollerStyle, lv_color_black());
    lv_style_set_radius(&activeRollerStyle, 4);
    lv_style_set_pad_all(&activeRollerStyle, 3);

    lv_style_t rollerBgStyle;
    lv_style_init(&rollerBgStyle);
    lv_style_set_text_color(&rollerBgStyle, lv_color_white());
    lv_style_set_bg_color(&rollerBgStyle, lv_color_black());
    lv_style_set_border_width(&rollerBgStyle, 0);
    lv_style_set_border_color(&rollerBgStyle, lv_color_black());
    lv_style_set_radius(&rollerBgStyle, 4);
    lv_style_set_pad_all(&rollerBgStyle, 3);

    lv_obj_t *titleLabel = lv_label_create(lv_screen_active());
    lv_label_set_text(titleLabel, "Mhz");
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, -70);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(titleLabel, lv_font_default(), 0);

    lv_obj_t *mhzRoller = lv_roller_create(lv_screen_active());
    lv_roller_set_options(mhzRoller, rollerMhzOpts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(mhzRoller, 1);
    lv_obj_add_style(mhzRoller, &rollerBgStyle, LV_PART_ANY);
    lv_obj_add_style(mhzRoller, &activeRollerStyle, LV_PART_SELECTED);
    lv_obj_align(mhzRoller, LV_ALIGN_CENTER, -63, 25);
    lv_obj_clear_flag(mhzRoller, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(mhzRoller, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(mhzRoller, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, ".");
    lv_obj_align(label, LV_ALIGN_CENTER, -23, 25);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, lv_font_default(), 0);

    lv_obj_t *dkhzRoller = lv_roller_create(lv_screen_active());
    lv_roller_set_options(dkhzRoller, rollerdkhzOpts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(dkhzRoller, 1);
    lv_obj_add_style(dkhzRoller, &rollerBgStyle, LV_PART_ANY);
    lv_obj_add_style(dkhzRoller, &inactiveRollerStyle, LV_PART_SELECTED);
    lv_obj_align(dkhzRoller, LV_ALIGN_CENTER, 8, 25);
    lv_obj_clear_flag(dkhzRoller, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(dkhzRoller, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(dkhzRoller, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t *cmkhzRoller = lv_roller_create(lv_screen_active());
    lv_roller_set_options(cmkhzRoller, rollercmkhzOpts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(cmkhzRoller, 1);
    lv_obj_add_style(cmkhzRoller, &rollerBgStyle, LV_PART_ANY);
    lv_obj_add_style(cmkhzRoller, &inactiveRollerStyle, LV_PART_SELECTED);
    // lv_obj_set_style_bg_opa(cmkhzRoller, LV_OPA_TRANSP, LV_PART_SELECTED);
    lv_obj_align(cmkhzRoller, LV_ALIGN_CENTER, 65, 25);
    lv_obj_clear_flag(cmkhzRoller, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cmkhzRoller, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(cmkhzRoller, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    // initArc();

    lv_group_t *arcGroup = lv_group_create();
    // lv_group_set_default(group);
    lv_group_add_obj(arcGroup, valueArc);
    lv_group_set_editing(arcGroup, true);

    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev != nullptr; indev = lv_indev_get_next(indev))
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER)
        {
            lv_indev_set_group(indev, arcGroup);
            break;
        }
    }

    // lv_obj_add_event_cb(valueArc, KnobVHF::vhfEventKnobCallback, LV_EVENT_ALL, nullptr);
}

#endif