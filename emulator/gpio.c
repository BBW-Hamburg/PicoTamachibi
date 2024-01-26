#include "emulator.h"

#include <stdbool.h>
#include <pico/stdlib.h>



void gpio_init(unsigned gpio) {
    emu_button_add(gpio);
}

void gpio_deinit(unsigned gpio) {
    emu_button_remove(gpio);
}

bool gpio_get(unsigned gpio) {
    return emu_button_is_held(gpio);
}

void gpio_set_irq_enabled_with_callback(unsigned gpio, unsigned a, bool b, typeof(gpio_set_irq_enabled_with_callback_t) handler) {
    // Ignore unused arguments
    (void)a;
    (void)b;

    emu_button_register_callback(gpio, handler);
}
