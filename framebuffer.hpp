#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <array>
#include <span>
#include <cstdint>


class Framebuffer {
    const unsigned width, height;

    bool readonly = false;

public:
    using Data = std::span<char>;
    using ROData = std::span<const char>;

    struct Position {
        unsigned byte;
        uint8_t bit;

        Position(unsigned x, unsigned y, unsigned width) {
            const unsigned raw = width * y + x;
            byte = raw/8;
            bit = raw%8;
        }

        Position(const Framebuffer& parent, unsigned x, unsigned y)
            : Position(x, y, parent.width) {}
    };

    Data data;

    Framebuffer(Data data, unsigned width, unsigned height)
        : data(data), width(width), height(height) {}
    Framebuffer(unsigned width, unsigned height)
        : width(width), height(height) {}

    unsigned get_buffer_size() const {
        return width * height / 8;
    }

    [[nodiscard]] bool load(Data d);
    [[nodiscard]] bool load_ro(ROData d);

    bool get(Position pos) const {
        return data[pos.byte] & (0b10000000 >> pos.bit);
    }
    void set(Position pos, bool value = true);
    void flip(Position pos);

    void invert();

    void blit(const Framebuffer& fbuf, unsigned x, unsigned y);

    unsigned get_width() const {
        return width;
    }
    unsigned get_height() const {
        return height;
    }
};
#endif // FRAMEBUFFER_HPP
