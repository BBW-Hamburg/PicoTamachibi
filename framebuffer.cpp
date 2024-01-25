#include "framebuffer.hpp"
#include "context.hpp"



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

void Framebuffer::set(Position pos, bool value) {
    if (readonly)
        Context::get().panic("Wr to rw FB");

    auto& byte = data[pos.byte];
    if (value)
        byte |= (0b10000000 >> pos.bit);
    else
        byte &= ~(0b10000000 >> pos.bit);
}

void Framebuffer::flip(Position pos) {
    if (readonly)
        Context::get().panic("Wr to rw FB");

    data[pos.byte] ^= (0b10000000 >> pos.bit);
}

void Framebuffer::invert() {
    if (readonly)
        Context::get().panic("Wr to rw FB");

    for (auto& byte : data) {
        byte = ~byte;
    }
}

void Framebuffer::blit(const Framebuffer &fbuf, unsigned int x, unsigned int y) {
    if (readonly)
        return;

    // Start in top left corner
    unsigned x_off = 0,
            y_off = 0;

    // Set each
    while (true) {
        set({*this, x + x_off, y + y_off}, fbuf.get({fbuf, x_off, y_off}));
        //set({*this, x + x_off, y + y_off}, x_off&1 && y_off&1);

        // Next pixel
        if (++x_off == fbuf.width) {
            x_off = 0;
            if (++y_off == fbuf.height)
                break;
        }
    }
}
