#include "hue.h"

#define canvas_buffer_size ((TFT_WIDTH * TFT_WIDTH) * sizeof(uint16_t))
static lv_color_t *canvas_buffer = NULL;

void drawColorWheel(int xc, int yc, int inner_radius, int width, lv_obj_t *canvas)
{
    uint16_t outer_radius = inner_radius + width;
    float angle_step = 0.001f; // Smaller step for smoother arcs
    float start_rad = 0 * DEG_TO_RAD;
    float end_rad = 360 * DEG_TO_RAD;

    for (int r = inner_radius; r <= outer_radius; r++)
    {
        for (float theta = start_rad; theta <= end_rad; theta += angle_step)
        {
            float x = r * cos(theta);
            float y = r * sin(theta);

            uint16_t x_pixel = static_cast<int>(round(xc + x));
            uint16_t y_pixel = static_cast<int>(round(yc + y));

            uint16_t hue = static_cast<uint16_t>(theta * 180 / M_PI + 90) % 360;

            lv_color_t arc_color = lv_color_hsv_to_rgb(hue, 100, 100);

            lv_canvas_set_px(canvas, x_pixel, y_pixel, arc_color);
        }
        delay(1);
    }

    lv_obj_invalidate(canvas);
}

HuePage::HuePage(lv_obj_t *parent) : BasePage(parent)
{
    const uint16_t color_wheel_inner_diameter = 195;
    const uint16_t color_wheel_width = 10;

    canvas_buffer = (lv_color_t *)heap_caps_aligned_alloc(4, canvas_buffer_size, MALLOC_CAP_SPIRAM);
    assert(canvas_buffer != NULL);

    memset(canvas_buffer, 0, canvas_buffer_size);

    lv_obj_t *canvas = lv_canvas_create(page);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(canvas, canvas_buffer, TFT_WIDTH, TFT_WIDTH, LV_IMG_CF_TRUE_COLOR);

    lv_coord_t radius = color_wheel_inner_diameter / 2;
    lv_coord_t center = TFT_WIDTH / 2;

    auto taskLambda = [center, radius, color_wheel_width, canvas]()
    {
        // Call the drawColorWheel function with the captured parameters
        drawColorWheel(center, center, radius, color_wheel_width, canvas);

        // Task completion: delete the task (if necessary)
        vTaskDelete(NULL);
    };

    // Create the FreeRTOS task using the lambda
    xTaskCreatePinnedToCore(
        [](void *taskFunc)
        {
            // Call the lambda function (cast taskFunc back to std::function)
            auto func = static_cast<std::function<void()> *>(taskFunc);
            (*func)();   // Execute the lambda function
            delete func; // Clean up after task is done
            vTaskDelete(NULL);
        },
        "DrawColorWheelLambda",                // Name of the task
        4096,                                  // Stack size
        new std::function<void()>(taskLambda), // Pass the lambda as the task parameter
        0,                                     // Priority
        NULL,                                  // Task handle (optional)
        0);

    // drawColorWheel(center, center, radius, color_wheel_width, canvas);

    hue_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(hue_selector, lv_color_hsv_to_rgb(0 * skip_degrees_selectable, 100, 100), LV_PART_MAIN);
    lv_obj_align(hue_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + 0)), selector_radius * sin(deg_1_rad * (270 + 0)));
}

void HuePage::update(xSemaphoreHandle mutex, int16_t position)
{
    int16_t app_hue_deg = position * skip_degrees_selectable;

    if (app_hue_deg < 0)
        hsv.h = 360 + app_hue_deg % 360;
    else
        hsv.h = app_hue_deg % 360;

    float u = DEG_TO_RAD * (270 + app_hue_deg);

    uint16_t x = selector_radius * cos(u);
    uint16_t y = selector_radius * sin(u);

    SemaphoreGuard lock(mutex);
    lv_obj_set_style_bg_color(hue_selector, lv_color_hsv_to_rgb(hsv.h, 100, 100), LV_PART_MAIN);
    lv_obj_align(hue_selector, LV_ALIGN_CENTER, x, y);
}