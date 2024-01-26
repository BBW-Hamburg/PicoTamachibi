#ifndef DISPLAY_HPP
#define DISPLAY_HPP
extern "C" {
#include "ssd1306.h"
}
#include "font.h"
#include "framebuffer.hpp"

#include <hardware/i2c.h>
#include <etl/string_view.h>
#include <etl/span.h>


class Display {
    ssd1306_t disp{};

public:
    struct Coord {
        unsigned x, y;
    };
    struct Size {
        unsigned width, height;
    } static constexpr size {128, 64};

    Display(unsigned sda, unsigned scl, i2c_inst *i2c);

    void end_frame();
    void begin_frame();

    void render_pixel(Coord coord);
    void render_text(Coord coord,  etl::string_view text);
    void render_framebuffer(const Framebuffer&);

    void fullframe_blank();
    void fullframe_framebuffer(const Framebuffer&);
    void fullframe_text( etl::string_view text);
    void fullframe_text( etl::span<const  etl::string_view> text);
    void fullframe_text_simple( etl::string_view text);

    static inline constexpr unsigned get_pixel_width_of_text(unsigned char_count) {
        return char_count * (FONT_WIDTH+1) - 1;
    }
    static inline constexpr unsigned get_pixel_height_of_text(unsigned lines) {
        return lines * (FONT_HEIGHT+1) - 1;
    }

    static inline constexpr unsigned get_characters_per_line() {
        return size.width / (FONT_WIDTH+1);
    }
    static inline constexpr unsigned get_characters_per_column() {
        return size.height / (FONT_HEIGHT);
    }
    static inline constexpr unsigned get_characters_per_frame() {
        return get_characters_per_line() * get_characters_per_column();
    }
};
#endif // DISPLAY_HPP
