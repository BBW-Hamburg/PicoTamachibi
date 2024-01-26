#include "framebuffer.hpp"
#include "context.hpp"
#include "font.h"

#include <cstring>



bool Framebuffer::load(Data d) {
    if (d.size() < get_buffer_size())
        return false;

    data = d;
    readonly = false;
    return true;
}

bool Framebuffer::load_ro(ROData d) {
    if (d.size() < get_buffer_size())
        return false;

    data = {const_cast<char*>(d.data()), d.size()};
    readonly = true;
    return true;
}

bool Framebuffer::get(Position pos) const {
    ASSERT_PANIC("Bad FB coord", pos.byte < data.size());

    return data[pos.byte] & (0b10000000 >> pos.bit);
}

void Framebuffer::set(Position pos, bool value) {
    ASSERT_PANIC("Wr to ro FB", !readonly);
    ASSERT_PANIC("Bad FB coord", pos.byte < data.size());

    auto& byte = data[pos.byte];
    if (value)
        byte |= (0b10000000 >> pos.bit);
    else
        byte &= ~(0b10000000 >> pos.bit);
}

void Framebuffer::flip(Position pos) {
    ASSERT_PANIC("Wr to ro FB", !readonly);
    ASSERT_PANIC("Bad FB coord", pos.byte < data.size());

    data[pos.byte] ^= (0b10000000 >> pos.bit);
}

void Framebuffer::vline(unsigned x, unsigned y_start, unsigned y_end) {
    for (unsigned y = y_start; y != y_end; y++)
        set({x, y, width});
}

void Framebuffer::hline(unsigned x_start, unsigned x_end, unsigned y) {
    for (unsigned x = x_start; x != x_end; x++)
        set({x, y, width});
}

void Framebuffer::rect(unsigned x_start, unsigned x_end, unsigned y_start, unsigned y_end) {
    vline(x_start, y_start, y_end);
    vline(x_end, y_start, y_end);
    hline(x_start, x_end, y_start);
    hline(x_start, x_end, y_end);
}

void Framebuffer::clear() {
    ASSERT_PANIC("Wr to ro FB", !readonly);
    memset(data.data(), 0, data.size_bytes());
}

void Framebuffer::blit(const Framebuffer &fbuf, unsigned int x, unsigned int y) {
    if (readonly)
        return;

    // Check if inversion is needed
    const bool invert = fbuf.inverted != inverted;

    // Start in top left corner
    unsigned x_off = 0,
            y_off = 0;

    // Set each
    while (true) {
        set({*this, x + x_off, y + y_off}, fbuf.get({fbuf, x_off, y_off}) != invert);
        //set({*this, x + x_off, y + y_off}, x_off&1 && y_off&1);

        // Next pixel
        if (++x_off == fbuf.width) {
            x_off = 0;
            if (++y_off == fbuf.height)
                break;
        }
    }
}

void Framebuffer::overlay(const Framebuffer &fbuf) {
    if (readonly)
        return;

    // Check if inversion is needed
    const bool invert = fbuf.inverted != inverted;

    // Start in top left corner
    unsigned x = 0,
            y = 0;

    // Set each
    while (true) {
        if (fbuf.get({fbuf, x, y}) != invert)
            set({*this, x, y});

        // Next pixel
        if (++x == fbuf.width) {
            x = 0;
            if (++y == fbuf.height)
                break;
        }
    }
}
