#ifndef EVENT_HPP
#define EVENT_HPP
#include "icon.hpp"
#include "framebuffer.hpp"

#include <etl/string.h>
#include <etl/string_view.h>


class Event {
    Image sprite;
    etl::string<16> message;

public:
    Event(const Image& sprite)
        : sprite(sprite) {}

    etl::string_view get_message() const {
        return message;
    }
    void set_message(const etl::string<16>& v) {
        message = v;
    }

    void popup(Framebuffer& fbuf, class Display& oled);
};

#endif // EVENT_HPP
