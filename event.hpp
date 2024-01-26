#ifndef EVENT_HPP
#define EVENT_HPP
#include "icon.hpp"

#include <etl/string.h>
#include <etl/string_view.h>


class Event {
    etl::string_view name;
    unsigned value;
    Icon sprite;
    int timer = -1; // -1 means no timer set
    uint32_t timer_elapsed = 0;
    etl::string<16> message;

public:
    Event(const Icon& sprite, etl::string_view name = "", unsigned value = 0)
        : name(name), value(value), sprite(sprite) {}

    etl::string_view get_name() const {
        return name;
    }
    void set_name(etl::string_view v) {
        name = v;
    }

    unsigned get_value() const {
        return value;
    }
    void set_value(unsigned v) {
        value = v;
    }

    const Icon& get_sprite() const {
        return sprite;
    }
    Icon& get_sprite() {
        return sprite;
    }

    etl::string_view get_message() const {
        return message;
    }
     void set_message(const etl::string<16>& v) {
        message = v;
    }

    void popup(class Display& oled);

    int get_timer() const {
        return timer;
    }
    void set_timer(int v) {
        timer = v;
    }
    void unset_timer() {
        timer = -1;
        reset_elapsed();
    }

    int get_elapsed() const {
        return timer_elapsed;
    }
    void reset_elapsed() {
        timer_elapsed = 0;
    }

    /// Progresses the animation on frame
    void tick();
};

#endif // EVENT_HPP
