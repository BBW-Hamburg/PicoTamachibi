#include "human_input.hpp"
#include "context.hpp"

#include <array>
#include <utility>
#include <pico/stdlib.h>



static inline uint32_t get_time_ms() {
    return us_to_ms(time_us_64());
}


static std::array<struct Button*, 8> buttons;


Button::Button(unsigned gpio) : gpio(gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_down(gpio);
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE, true, irq_handler);

    // Add button to list
    for (auto& button : buttons) {
        if (button == nullptr) {
            button = this;
            return;
        }
    }

    // No slot found, panic
    Context::get().panic("Btn pool full");
}

Button::~Button() {
    gpio_deinit(gpio);
    // Remove button from list
    for (auto& button : buttons) {
        if (button == this) {
            button = nullptr;
            break;
        }
    }
}

void Button::irq_handler(unsigned gpio, uint32_t event_mask) {
    for (auto& button : buttons) {
        if (button && button->gpio == gpio) {
            if (get_time_ms() - button->last_push > 400) {
                button->pushed = true;
                button->last_push = get_time_ms();
                break;
            }
        }
    }
}

void Button::clear_all() {
    for (auto& button : buttons) {
        if (button) {
            button->clear();
        }
    }
}

bool Button::was_pushed() const {
    return std::exchange(pushed, false);
}
bool Button::is_held() const {
    return gpio_get(gpio);
}

void Button::clear() {
    pushed = false;
}
