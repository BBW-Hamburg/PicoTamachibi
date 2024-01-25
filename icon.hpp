#ifndef ICON_HPP
#define ICON_HPP
#include "context.hpp"
#include "display.hpp"
#include "framebuffer.hpp"

#include <string_view>
#include <vector>
#include <variant>


class Icon {
    Framebuffer image;
    unsigned x,
             y;
    bool inverted = false;
    std::string_view name;

public:
    Icon(const char *filename, unsigned width = 16, unsigned height = 16, unsigned x = 0, unsigned y = 0, std::string_view name = "Empty")
          : image(width, height), x(x), y(y), name(name) {
        if (!image.load_ro(load_icon(filename)))
            Context::get().panic("Fail Img load");
    }

    const Framebuffer& get_image() const {
        return image;
    }
    void set_image(const Framebuffer::ROData& buf) {
        if (!image.load_ro(buf))
            Context::get().panic("Fail Img load");
    }

    unsigned get_x() const {
        return x;
    }
    void set_x(unsigned v) {
        x = v;
    }
    unsigned get_y() const {
        return y;
    }
    void set_y(unsigned v) {
        y = v;
    }

    unsigned get_width() const {
        return image.get_width();
    }
    unsigned get_height() const {
        return image.get_height();
    }

    std::string_view get_name() const {
        return name;
    }
    void get_name(std::string_view& v) {
        name = v;
    }

    bool is_inverted() const {
        return inverted;
    }
    void set_inverted(bool v);

    Framebuffer::ROData load_icon(const char *filename);
};


class OptionallyAnimatedIcon : public std::variant<Icon, std::nullptr_t> {
public:
    void set_inverted(bool v) {
        switch (index()) {
        case 0: {
            std::get<0>(*this).set_inverted(v);
        } break;
        case 1: {
            //TODO: Animation
        } break;
        }
    }

    std::string_view get_name() const {
        switch (index()) {
        case 0: {
            std::get<0>(*this).get_name();
        } break;
        case 1: {
            //TODO: Animation
        } break;
        }
        return "Empty";
    }
};


class Toolbar {
    std::vector<OptionallyAnimatedIcon> icon_array;
    std::vector<char> framebuf_data;
    Framebuffer framebuf;
    unsigned spacer = 1;
    int selected_index = -1;

public:
    Toolbar()
          : framebuf_data(160*16/8), framebuf(160, 16) {
        if (!framebuf.load(framebuf_data))
            Context::get().panic("Tb FB error");
    }
    Toolbar(const Toolbar&) = delete;
    Toolbar(Toolbar&&) = delete;

    void add_item(OptionallyAnimatedIcon icon) {
        icon_array.emplace_back(std::move(icon));
    }

    const Framebuffer& generate_data();

    unsigned get_spacer() const {
        return spacer;
    }
    void set_spacer(unsigned v) {
        spacer = v;
    }

    void show(Display& oled) {
        oled.render_framebuffer(generate_data());
    }

    void select(unsigned index, Display& oled) {
        // Invert icon
        icon_array[index].set_inverted(true);

        // Update self
        selected_index = index;

        // Update display
        show(oled);
    }

    void unselect(Display& oled) {
        // Un-invert icon
        icon_array[selected_index].set_inverted(false);

        // Update self
        selected_index = -1;

        // Update display
        show(oled);
    }

    std::string_view selected_item() const {
        return icon_array[selected_index].get_name();
    }
};
#endif // ICON_HPP
