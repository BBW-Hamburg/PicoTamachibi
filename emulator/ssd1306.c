#include "emulator.h"
#include "../ssd1306.h"



bool ssd1306_init(ssd1306_t *p, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c_instance) {
    return emu_display_enable(width, height);
}
void ssd1306_deinit(ssd1306_t *p) {
    emu_display_disable();
}

void ssd1306_show(ssd1306_t *p) {
    emu_display_show();
}
void ssd1306_clear(ssd1306_t *p) {
    emu_display_clear();
}

void ssd1306_clear_pixel(ssd1306_t *p, uint32_t x, uint32_t y) {
    emu_display_set_pixel(x, y, false);
}

void ssd1306_draw_pixel(ssd1306_t *p, uint32_t x, uint32_t y) {
    emu_display_set_pixel(x, y, true);
}
