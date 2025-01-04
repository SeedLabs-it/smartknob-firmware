Will contain instructions for font conversion on lvgl.
English alphabet:
0x20-0x7E

Nordic letters:
0xC0-0xFF

Symbols:
°,’



pnpm install lv_font_conv

lv_font_conv --bpp 8 --size 10 --no-compress --font AktivGrotesk-Regular.otf --symbols °,’ --range 32-126,192-255 --format lvgl -o aktivgrotesk_regular_10pt.c