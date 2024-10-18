#include "hue.h"

static lv_obj_t *meter;
static lv_color_t *canvas_buffer = NULL;

static void meter_draw_event_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    if (dsc->type == LV_METER_DRAW_PART_TICK)
    {
        // LOGE("dsc->id: %d", dsc->id);
        // dsc->line_dsc->color = lv_color_hsv_to_rgb(dsc->id * skip_degrees_def, 100, 100);
    }
}

void drawArc(int xc, int yc, int innerRadius, int thickness, int startAngle, int endAngle, lv_obj_t *canvas)
{
    int outerRadius = innerRadius + thickness;
    float angleStep = 0.001f; // Smaller step for smoother arcs
    float startRad = startAngle * M_PI / 180.0f;
    float endRad = endAngle * M_PI / 180.0f;

    // Loop through the range of radii from the innerRadius to the outerRadius
    for (int r = innerRadius; r <= outerRadius; r++)
    {
        for (float theta = startRad; theta <= endRad; theta += angleStep)
        {
            // Calculate pixel coordinates in floating point
            float x = r * cos(theta);
            float y = r * sin(theta);

            // Integer coordinates
            int xInt = static_cast<int>(round(xc + x));
            int yInt = static_cast<int>(round(yc + y));

            // Calculate the hue based on the angle
            int hue = static_cast<int>(theta * 180 / M_PI + 180) % 360;

            // Convert HSV to RGB (100% saturation and brightness)
            lv_color_t arcColor = lv_color_hsv_to_rgb(hue, 100, 100);

            // Default opacity is fully opaque (for non-edge pixels)
            float finalOpacity = 1.0f;

            // Antialias only the inner and outer radius edges
            if (r == innerRadius)
            {
                // Apply aliasing to inner edge by reducing opacity
                finalOpacity = 0.5f; // Adjust for smoother inner edge
            }
            else if (r == outerRadius)
            {
                // Apply aliasing to outer edge by reducing opacity
                finalOpacity = 0.5f; // Adjust for smoother outer edge
            }

            // If opacity is full, directly set the pixel without blending
            if (finalOpacity == 1.0f)
            {
                lv_canvas_set_px(canvas, xInt, yInt, arcColor); // Direct pixel placement with no blending
            }
            else
            {
                // Blend the color with black for the edge pixels
                lv_color_t mod_color = lv_color_mix(arcColor, lv_color_black(), (1.0f - finalOpacity) * 255);
                lv_canvas_set_px(canvas, xInt, yInt, mod_color); // Place blended edge pixels
            }
        }
    }
}

// void smoothEdges(std::vector<Pixel> &arcPixels, int width, int height)
// {
//     // Example: Simple blur algorithm to blend edge pixels (a more complex blur can be applied)

//     for (int i = 1; i < arcPixels.size() - 1; i++)
//     {
//         // Blend current pixel opacity with the previous and next pixels
//         arcPixels[i].opacity = (arcPixels[i - 1].opacity + arcPixels[i].opacity + arcPixels[i + 1].opacity) / 3.0f;
//     }
// }

#define canvas_buffer_size ((TFT_WIDTH * TFT_WIDTH) * sizeof(uint16_t))

HuePage::HuePage(lv_obj_t *parent) : BasePage(parent)
{
    const uint16_t color_wheel_inner_diameter = 195;
    const uint16_t color_wheel_width = 10;

    const uint8_t line_thickness = 4;

    canvas_buffer = (lv_color_t *)heap_caps_aligned_alloc(4, canvas_buffer_size, MALLOC_CAP_SPIRAM);
    assert(canvas_buffer != NULL);

    memset(canvas_buffer, 0, canvas_buffer_size);

    lv_obj_t *canvas = lv_canvas_create(page);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(canvas, canvas_buffer, TFT_WIDTH, TFT_WIDTH, LV_IMG_CF_TRUE_COLOR);

    // lv_coord_t radius = color_wheel_diameter / 2;
    // lv_coord_t center_x = radius;
    // lv_coord_t center_y = radius;
    // TFT_WIDTH / 2 - color_wheel_diameter / 2

    lv_coord_t radius = color_wheel_inner_diameter / 2;
    lv_coord_t center = TFT_WIDTH / 2;

    // Iterate over all angles to draw the color wheel
    // for (int angle = 0; angle < 360; angle++)
    // {
    //     // Calculate the color at the current angle (hue value from HSV)
    //     lv_color_t color = lv_color_hsv_to_rgb(angle, 100, 100); // Full saturation and value for vibrant colors

    //     // Calculate angle in radians
    //     float angle_rad = angle * DEG_TO_RAD;

    //     // Take width into account to draw the line from start of width to end of diameter
    //     int x1 = center_x + (int)((radius - color_wheel_width / 2 - line_thickness) * cos(angle_rad));
    //     int y1 = center_y + (int)((radius - color_wheel_width / 2 - line_thickness) * sin(angle_rad));

    //     // Calculate the end point of the radial line
    //     int x2 = center_x + (int)((radius - line_thickness) * cos(angle_rad));
    //     int y2 = center_y + (int)((radius - line_thickness) * sin(angle_rad));

    //     // Draw the radial line using Bresenham's algorithm
    //     draw_line(canvas, x1, y1, x2, y2, color, line_thickness, true); // Adjust 'thickness' value as needed
    // }

    drawArc(center, center, radius, color_wheel_width, 0, 360, canvas);
    // smoothEdges(d, TFT_WIDTH, TFT_WIDTH);
    // std::vector<Pixel> lowResArc = downsampleArc(d, scaleFactor);
    // for (Pixel p : d)
    // {
    //     // calculate color with opacity
    //     // lv_color_t color = lv_color_hsv_to_rgb(p.y, 100, 100); // this doesnt go round in a circle with the color

    //     // go round in a circle with the color using x and y
    //     lv_color_t color = lv_color_hsv_to_rgb(atan2(p.y - center, p.x - center) * 180 / M_PI + 180, 100, 100);
    //     lv_color_t mod_color = lv_color_mix(color, lv_color_black(), p.opacity * 255);
    //     lv_canvas_set_px(canvas, p.x, p.y, mod_color);
    //     // lv_canvas_set_px(canvas, p.x, p.y, lv_color_hsv_to_rgb(p.y, 100, 100));
    // }

    // After the drawing loop, invalidate the canvas to refresh it
    lv_obj_invalidate(canvas);

    hue_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(hue_selector, lv_color_hsv_to_rgb(0 * skip_degrees_selectable, 100, 100), LV_PART_MAIN);
    lv_obj_align(hue_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + 0)), selector_radius * sin(deg_1_rad * (270 + 0)));
}

void HuePage::update(xSemaphoreHandle mutex, int16_t position)
{
    int16_t app_hue_deg = position * skip_degrees_selectable;

    hsv.h = app_hue_deg;
    hsv.s = 100;
    hsv.v = 100;

    float u = deg_1_rad * (270 + app_hue_deg);

    uint16_t x = selector_radius * cos(u);
    uint16_t y = selector_radius * sin(u);

    SemaphoreGuard lock(mutex);
    lv_obj_set_style_bg_color(hue_selector, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), LV_PART_MAIN);
    lv_obj_align(hue_selector, LV_ALIGN_CENTER, x, y);
}