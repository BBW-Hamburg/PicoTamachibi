#include <pico/stdlib.h>
#include <pico/bootrom.h>


class Input {
    const unsigned gpio;

public:
    Input(unsigned gpio, bool pull_up = true) : gpio(gpio) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_IN);
        if (pull_up)
            gpio_pull_up(gpio);
    }

    bool is_pushed() const {
        return !gpio_get(gpio);
    }
    bool is_activated() const {
        return gpio_get(gpio);
    }
} power_button(14), reset_button(15), power_led_input(19, false);


class Output {
    const unsigned gpio;

public:
    Output(unsigned gpio) : gpio(gpio) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
        set_activated(false);
    }

    void set_activated(bool active) {
        gpio_put(gpio, active);
    }
} power_led_output(20);


class PowerSwitch {
    const unsigned gpio;

public:
    PowerSwitch(unsigned gpio) : gpio(gpio) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
        set_pushed(false);
    }

    void set_pushed(bool pushed) {
        gpio_put(gpio, !pushed);
    }

    void do_hard_reset() {
        power_led_output.set_activated(false);
        set_pushed(true);
        sleep_ms(4200);
        set_pushed(false);
        sleep_ms(1000);
        set_pushed(true);
        sleep_ms(200);
        set_pushed(false);
    }

    void do_hard_power_off() {
        power_led_output.set_activated(false);
        set_pushed(true);
        sleep_ms(4200);
        set_pushed(false);
    }

    void do_short_push() {
        const bool led_activated = power_led_input.is_activated();
        power_led_output.set_activated(!led_activated);
        set_pushed(true);
        sleep_ms(200);
        set_pushed(false);
        power_led_output.set_activated(led_activated);
        sleep_ms(150);
    }
} power_switch(16);


class OfflineHandler {
    unsigned last_released;
    bool handled;

    static inline uint32_t get_time_ms() {
        return us_to_ms(time_us_64());
    }

public:
    OfflineHandler() {
        reset();
    }

    void reset() {
        last_released = 0;
        handled = true;
    }

    void run() {
        // Handle reset button being pushed
        if (reset_button.is_pushed()) {
            power_switch.do_hard_reset();
        }
        // Handle power button release
        if (!power_button.is_pushed()) {
            if (!handled) {
                // Check timer
                uint32_t hold_duration = get_time_ms() - last_released;
                if (hold_duration >= 15000) {
                    reset_usb_boot(0, 0);
                    __unreachable();
                    while (true) asm("deadcode: jmp deadcode");
                } else if (hold_duration >= 4000) {
                    power_switch.do_hard_reset();
                } else if (hold_duration >= 2000) {
                    power_switch.do_hard_power_off();
                } else {
                    power_switch.do_short_push();
                }
                // Mark as handled
                handled = true;
            }
            // Update timer
            last_released = get_time_ms();
            // Synchronize power LED
            power_led_output.set_activated(power_led_input.is_activated());
        } else {
            // Mark next release as not yet handled
            handled = false;
            // Blink
            power_led_output.set_activated((((get_time_ms()-last_released)/500) % 2) == 1);
        }
    }
} offline_handler;


class SerialHandler {
    uart_inst_t *uart = uart1;

public:
    void run() {
        // Don't try to read if there's nothing
        if (!uart_is_readable(uart))
            return;

        // Read and execute command
        switch (uart_getc(uart)) {
        case 'S': power_switch.do_short_push(); break;
        case 'O': power_switch.do_hard_power_off(); break;
        case 'R': power_switch.do_hard_reset(); break;
        }
    }
} serial_handler;


int main() {
    for (;;) {
        sleep_ms(50);
        offline_handler.run();
        serial_handler.run();
    }
}
