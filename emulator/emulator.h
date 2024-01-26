#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <pico/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Display
bool emu_display_enable(unsigned width, unsigned height);
void emu_display_disable();
void emu_display_clear();
void emu_display_set_pixel(unsigned x, unsigned y, bool v);
void emu_display_show();

// Buttons
void emu_button_add(int id);
bool emu_button_is_held(int id);
void emu_button_register_callback(int id, typeof(gpio_set_irq_enabled_with_callback_t) *callback);
bool emu_button_remove(int id);

#ifdef __cplusplus
}
#endif
