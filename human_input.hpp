#ifndef HUMAN_INPUT_HPP
#define HUMAN_INPUT_HPP
#include <cstdint>


class Button {
    static void irq_handler(unsigned gpio, uint32_t event_mask);

    uint32_t last_push = 0;
    unsigned gpio;
    mutable bool pushed = false;

public:
    static void clear_all();

    Button(unsigned gpio);
    ~Button();

    Button(const Button&) = delete;
    Button(Button&&) = delete;

    unsigned get_gpio() const {
        return gpio;
    }
    operator unsigned() const {
        return gpio;
    }

    bool was_pushed() const;
    bool is_held() const;
    void clear();
};
#endif // HUMAN_INPUT_HPP
