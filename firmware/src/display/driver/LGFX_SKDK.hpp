// #pragma once

// #define LGFX_USE_V1

// #include <LovyanGFX.hpp>

// static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

// class LGFX : public lgfx::LGFX_Device
// {
//     lgfx::Panel_GC9A01 _panel_instance;
//     lgfx::Bus_SPI _bus_instance;
//     lgfx::Light_PWM _light_instance;

// public:
//     LGFX(void)
//     {
//         {
//             auto cfg = _bus_instance.config();

//             cfg.spi_host = SPI3_HOST;

//             cfg.spi_mode = 0;
//             cfg.freq_write = 70000000;
//             cfg.freq_read = 16000000;
//             cfg.spi_3wire = true;
//             cfg.use_lock = true;
//             cfg.dma_channel = SPI_DMA_CH_AUTO;

//             cfg.pin_sclk = 4;
//             cfg.pin_mosi = 3;
//             cfg.pin_miso = -1;
//             cfg.pin_dc = 2;

//             _bus_instance.config(cfg);
//             _panel_instance.setBus(&_bus_instance);
//         }

//         {
//             auto cfg = _panel_instance.config();
//             cfg.pin_cs = 9;
//             cfg.pin_rst = 10;
//             cfg.pin_busy = -1;

//             cfg.panel_width = 240;
//             cfg.panel_height = 240;
//             cfg.offset_x = 0;
//             cfg.offset_y = 0;
//             cfg.offset_rotation = 0;
//             cfg.dummy_read_pixel = 8;
//             cfg.dummy_read_bits = 1;
//             cfg.readable = true;
//             cfg.invert = true;
//             cfg.rgb_order = false;
//             cfg.dlen_16bit = false;
//             cfg.bus_shared = true;

//             _panel_instance.config(cfg);
//         }

//         {
//             auto cfg = _light_instance.config();

//             cfg.pin_bl = PIN_LCD_BACKLIGHT;
//             cfg.invert = false;
//             cfg.freq = 44100;
//             cfg.pwm_channel = LEDC_CHANNEL_LCD_BACKLIGHT;

//             _light_instance.config(cfg);
//             _panel_instance.setLight(&_light_instance);
//         }

//         setPanel(&_panel_instance);
//     }
// };
