#include "display.hpp"
extern "C" {
#include "ssd1306.h"
}
#include "font.h"
#include "framebuffer.hpp"
#include "context.hpp"

#include <etl/utility.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>



static
etl::pair<etl::string_view, etl::string_view> split_once(etl::string_view s, etl::string_view delim) {
    // Find the delimiter
    auto pos = s.find(delim);
    if (pos == s.npos) {
        return {s, ""};
    }
    // Split there
    return {s.substr(0, pos), s.substr(pos+delim.size(), s.size()-1)};
}


Display::Display(unsigned sda, unsigned scl, i2c_inst *i2c) {
    // Initialize GPIO
    i2c_init(i2c, 400000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Initialize driver
    disp.external_vcc = false;
    ssd1306_init(&disp, size.width, size.height, 0x3C, i2c);
    ssd1306_clear(&disp);
}

void Display::end_frame() {
    ssd1306_show(&disp);
}

void Display::begin_frame() {
    ssd1306_clear(&disp);
}

void Display::render_pixel(Coord coord) {
    ssd1306_draw_pixel(&disp, coord.x, coord.y);
}

void Display::render_text(Coord coord, etl::string_view text) {
    for (auto c : text) {
        // Handle spaces quickly
        if (c == ' ') {
            coord.x += FONT_WIDTH + 1;
            continue;
        }

        // Get character in bitmap
        const bool *bitmap = font_get_character(c);

        // Render character
        unsigned x = 0;
        unsigned y = 0;
        for (const bool *thispixel = bitmap; ; thispixel++) {
            if (x == FONT_WIDTH) {
                x = 0;
                y++;
            }
            if (y == FONT_HEIGHT)
                break;

            if (*thispixel)
                render_pixel({coord.x+x, coord.y+y});

            x++;
        }

        // Go to next position
        coord.x += FONT_WIDTH + 1;
    }
}

void Display::render_framebuffer(const Framebuffer& fbuf) {
    // Start in top left corner
    Coord coord{0, 0};

    // Draw each pixel
    while (true) {
        if (fbuf.get({fbuf, coord.x, coord.y}) != fbuf.inverted)
            ssd1306_draw_pixel(&disp, coord.x, coord.y);

        // Next pixel
        if (++coord.x == size.width) {
            coord.x = 0;
            if (++coord.y == size.height)
                break;
        }
    }
}

void Display::fullframe_blank() {
    begin_frame();
    end_frame();
}

void Display::fullframe_framebuffer(const Framebuffer& fbuf) {
    if (fbuf.get_height() != size.height || fbuf.get_width() != size.width)
        Context::get().panic("Bad FB disp res");

    begin_frame();
    render_framebuffer(fbuf);
    end_frame();
}

void Display::fullframe_text(etl::string_view text) {
    // Begin new frame
    begin_frame();

    // Get middle
    Coord pos;
    pos.y = size.height/2 - FONT_HEIGHT/2;
    pos.x = size.width/2 - get_pixel_width_of_text(text.size())/2;

    // Render text
    render_text(pos, text);

    // End frame
    end_frame();
}

void Display::fullframe_text(etl::span<const etl::string_view> text) {
    // Begin new frame
    begin_frame();

    // Get middle
    Coord pos;
    pos.y = size.height/2 - get_pixel_height_of_text(text.size())/2;

    // Render text
    for (const auto line : text) {
        pos.x = size.width/2 - get_pixel_width_of_text(line.size())/2;
        render_text(pos, line);
        pos.y += FONT_HEIGHT;
    }

    // End frame
    end_frame();
}

void Display::fullframe_text_simple(etl::string_view text) {
    // Begin new frame
    begin_frame();

    // Get beginning
    Coord pos{0, 0};

    // Render text in chunks
    constexpr unsigned cpl = get_characters_per_line();
    for (unsigned start = 0, stop = false; !stop; start += cpl, pos.y += FONT_HEIGHT) {
        // Get text view to render
        etl::string_view text_view;
        if (text.size() - start > cpl) {
            text_view = {text.data()+start, cpl};
        } else {
            text_view = {text.data()+start, text.size() - start};
            stop = true;
        }
        // Render text
        render_text(pos, text_view);
    }

    // End frame
    end_frame();
}
