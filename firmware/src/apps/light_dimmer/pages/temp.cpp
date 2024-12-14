#include "temp.h"

#define canvas_buffer_size ((TFT_WIDTH * TFT_WIDTH) * sizeof(uint16_t))
static lv_color_t *canvas_buffer = NULL;

#include <math.h>
#include <stdint.h>

void drawKelvinWheel(int xc, int yc, int inner_radius, int width, lv_obj_t *canvas)
{
    int outer_radius = inner_radius + width;
    int x_start = xc - outer_radius - 1;
    int x_end = xc + outer_radius + 1;
    int y_start = yc - outer_radius - 1;
    int y_end = yc + outer_radius + 1;

    for (int y = y_start; y <= y_end; y++)
    {
        for (int x = x_start; x <= x_end; x++)
        {
            float dx = x - xc + 0.5f;
            float dy = y - yc + 0.5f;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance >= inner_radius - 0.5f && distance <= outer_radius + 0.5f)
            {
                float delta = 0.0f;
                if (distance < inner_radius)
                {
                    delta = inner_radius - distance;
                }
                else if (distance > outer_radius)
                {
                    delta = distance - outer_radius;
                }

                float intensity = 1.0f - fabsf(delta);
                intensity = intensity < 0.0f ? 0.0f : intensity;
                intensity = intensity > 1.0f ? 1.0f : intensity;

                float angle_dx = x - xc;
                float angle_dy = y - yc;
                float theta = atan2f(angle_dy, angle_dx);
                if (theta < 0)
                {
                    theta += 2 * M_PI;
                }
                float t = theta / (2 * M_PI);

                float temp_t = 1.0f - fabsf(2.0f * t - 1.0f);

                uint16_t kelvin = temp_max + temp_t * (temp_min - temp_max);
                lv_color_t color = kelvinToLvColor(kelvin);

                lv_color_t blended_color = lv_color_mix(color, lv_color_make(0, 0, 0), intensity * 255);
                lv_canvas_set_px(canvas, x, y, blended_color);
            }
        }
    }
    lv_obj_invalidate(canvas);
}

TempPage::TempPage(lv_obj_t *parent, const AppData &app_data) : BasePage(parent)
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
        "DrawKelvinWheelLambda",
        4096,
        new std::function<void()>(taskLambda),
        0,
        NULL,
        0);

    temp_selector = lvDrawCircle(16, page);
    lv_obj_set_style_bg_color(temp_selector, kelvinToLvColor(temp_max), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad), selector_radius * sin(deg_1_rad));

    selected_temp_circle = lvDrawCircle(24, page);
    lv_obj_set_style_bg_color(selected_temp_circle, lv_color_black(), LV_PART_MAIN);
    lv_obj_align(selected_temp_circle, LV_ALIGN_CENTER, 0, 0);

    friendly_name_label_ = lv_label_create(page);
    lv_label_set_text(friendly_name_label_, app_data.friendly_name);
    lv_obj_align(friendly_name_label_, LV_ALIGN_CENTER, 0, 24);
}

void TempPage::update(xSemaphoreHandle mutex, int16_t position)
{
    int16_t app_temp_deg = (position * skip_degrees_selectable) % 360;
    uint16_t kelvin;

    uint16_t abs_temp_deg = abs(app_temp_deg);

    float normalized_angle = fmodf(abs(app_temp_deg), 360.0f) / 360.0f; // Normalize to [0, 1]

    if (normalized_angle <= 0.5f)
    {
        kelvin = temp_max + normalized_angle * 2 * (temp_min - temp_max); // Increase from 1000K to 10000K
    }
    else
    {
        kelvin = temp_min - (normalized_angle - 0.5f) * 2 * (temp_min - temp_max); // Decrease back to 1000K
    }

    lv_obj_set_style_bg_color(selected_temp_circle, kelvinToLvColor(kelvin), LV_PART_MAIN);

    lv_obj_set_style_bg_color(temp_selector, kelvinToLvColor(kelvin), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER,
                 selector_radius * cos(deg_1_rad * (app_temp_deg)),
                 selector_radius * sin(deg_1_rad * (app_temp_deg)));
}
