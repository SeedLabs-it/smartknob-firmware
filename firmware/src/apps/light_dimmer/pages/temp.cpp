#include "temp.h"

#define canvas_buffer_size ((TFT_WIDTH * TFT_WIDTH) * sizeof(uint16_t))
static lv_color_t *canvas_buffer = NULL;

void drawKelvinWheel(int xc, int yc, int inner_radius, int width, lv_obj_t *canvas)
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

            float normalized_angle = (theta - start_rad) / (end_rad - start_rad);

            uint16_t kelvin;

            if (normalized_angle <= 0.5f)
            {
                kelvin = temp_min + normalized_angle * 2 * (temp_max - temp_min);
            }
            else
            {
                kelvin = temp_max - (normalized_angle - 0.5f) * 2 * (temp_max - temp_min);
            }

            lv_color_t arc_color = kelvinToLvColor(kelvin);

            lv_canvas_set_px(canvas, x_pixel, y_pixel, arc_color);
        }
        delay(1);
    }
    lv_obj_invalidate(canvas);
}

TempPage::TempPage(lv_obj_t *parent) : BasePage(parent)
{
    const uint16_t temp_wheel_inner_diameter = 195;
    const uint16_t temp_wheel_width = 10;

    canvas_buffer = (lv_color_t *)heap_caps_aligned_alloc(4, canvas_buffer_size, MALLOC_CAP_SPIRAM);
    assert(canvas_buffer != NULL);

    memset(canvas_buffer, 0, canvas_buffer_size);

    lv_obj_t *canvas = lv_canvas_create(page);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(canvas, canvas_buffer, TFT_WIDTH, TFT_WIDTH, LV_IMG_CF_TRUE_COLOR);

    lv_coord_t radius = temp_wheel_inner_diameter / 2;
    lv_coord_t center = TFT_WIDTH / 2;

    auto taskLambda = [center, radius, temp_wheel_width, canvas]()
    {
        drawKelvinWheel(center, center, radius, temp_wheel_width, canvas);

        vTaskDelete(NULL);
    };

    xTaskCreatePinnedToCore(
        [](void *taskFunc)
        {
            auto func = static_cast<std::function<void()> *>(taskFunc);
            (*func)();
            delete func;
            vTaskDelete(NULL);
        },
        "DrawColorWheelLambda",
        4096,
        new std::function<void()>(taskLambda),
        0,
        NULL,
        0);

    temp_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(temp_selector, kelvinToLvColor(1000), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad), selector_radius * sin(deg_1_rad));
}

void TempPage::update(xSemaphoreHandle mutex, int16_t position)
{
    int16_t app_temp_deg = (position * skip_degrees_selectable) % 360;
    uint16_t kelvin;

    uint16_t abs_temp_deg = abs(app_temp_deg);

    float normalized_angle = fmodf(abs(app_temp_deg), 360.0f) / 360.0f; // Normalize to [0, 1]

    if (normalized_angle <= 0.5f)
    {
        kelvin = temp_min + normalized_angle * 2 * (temp_max - temp_min); // Increase from 1000K to 10000K
    }
    else
    {
        kelvin = temp_max - (normalized_angle - 0.5f) * 2 * (temp_max - temp_min); // Decrease back to 1000K
    }

    lv_obj_set_style_bg_color(temp_selector, kelvinToLvColor(kelvin), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER,
                 selector_radius * cos(deg_1_rad * (app_temp_deg)),
                 selector_radius * sin(deg_1_rad * (app_temp_deg)));
}
