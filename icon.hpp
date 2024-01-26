#ifndef ICON_HPP
#define ICON_HPP
#include "display.hpp"
#include "framebuffer.hpp"

#include <string_view>
#include <vector>
#include <variant>


class Icon {
    friend class Animate;

    Framebuffer image;
    unsigned x,
             y;
    bool inverted = false;
    std::string_view name;

public:
    Icon(const char *filename, unsigned width = 16, unsigned height = 16, unsigned x = 0, unsigned y = 0, std::string_view name = "Empty")
          : image(width, height), x(x), y(y), name(name) {
        set_image(load_icon(filename));
    }

    const Framebuffer& get_image() const {
        return image;
    }
    void set_image(const Framebuffer::ROData& buf);

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


class Animate {
    friend class Toolbar; // Toorbar is doing stupid things in generate_data(), so we gotta do this...

public:
    enum AnimationSpeed {
        very_slow,
        slow,
        normal,
        fast
    };

    enum AnimationType {
        default_,
        loop,
        bounce,
        reverse
    };

private:
    std::vector<Icon> frames;
    unsigned current_frame = 0;
    AnimationSpeed speed = normal;
    unsigned speed_value = 0;
    bool done = false;
    unsigned loop_count = 1;
    bool bouncing = false;
    AnimationType animation_type;
    unsigned pause = 0;
    bool active = false;
    unsigned x,
             y;
    unsigned width,
             height;
    bool cached = false;
    const char *filename;

public:
    Animate(const char *filename = NULL, AnimationType animation_type = default_, unsigned x = 0, unsigned y = 0, unsigned width = 16, unsigned height = 16, std::vector<Icon>&& frames = {})
        : frames(std::move(frames)), animation_type(animation_type), x(x), y(y), width(width), height(height), filename(filename) {}

    bool is_active() const {
        return active;
    }
    void set_active(bool v);

    AnimationSpeed get_speed() const {
        return speed;
    }
    void set_speed(AnimationSpeed v);

    AnimationType get_animation_type() const {
        return animation_type;
    }
    void set_animation_type(AnimationType v) {
        animation_type = v;
    }

    const char *get_filename() {
        return filename;
    }
    void set_filename(const char *v) {
        filename = v;
    }

    /// progress the current frame
    void do_forward();
    /// reversely progress the current frame
    void do_reverse();

    /// Load the animation files
    void load();
    /// Unload animation files
    void unload();

    /// Animates the frames based on the animation type and for the number of times specified
    void do_animate(Framebuffer& fbuf);

    unsigned get_frame_count() const {
        return frames.size() - 1;
    }

    bool is_done() {
        if (done) {
            done = false; // Accessing a "property" should NEVER have any side-effects!!!
            return true;
        } else {
            return false;
        }
    }

    void stop() {
        loop_count = 0;
        bouncing = false;
        done = true;
    }

    /// Loops the animation
    /// if no is -1 the animation will continue looping until stop() is called
    void do_loop(int no = -1) {
        if (no != -1)
            loop_count = no;
        else
            loop_count = -1;

        animation_type = loop;
    }

    /// Loops the animation forwared, then backward, the number of time specified in no,
    /// if no is -1 it will animate infinately
    void do_bounce(int no = -1) {
        if (no != -1)
            loop_count = no;
        else
            loop_count = -1;

        animation_type = bounce;
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
        return get_width();
    }
    unsigned get_height() const {
        return get_height();
    }
};


using OptionallyAnimatedIconBase = std::variant<Icon, Animate>;

class OptionallyAnimatedIcon : public OptionallyAnimatedIconBase {
public:
    using OptionallyAnimatedIconBase::OptionallyAnimatedIconBase;

    void set_inverted(bool v);
    std::string_view get_name() const;
};


class Toolbar {
    std::vector<OptionallyAnimatedIcon> icon_array;
    unsigned spacer = 1;
    int selected_index = -1;

public:
    Toolbar() {}

    Toolbar(const Toolbar&) = delete;

    Toolbar(Toolbar&& o)
        : icon_array(std::move(o.icon_array)), spacer(o.spacer), selected_index(o.selected_index) {}

    void add_item(OptionallyAnimatedIcon icon) {
        icon_array.emplace_back(std::move(icon));
    }

    void show(Framebuffer& fbuf);

    unsigned get_spacer() const {
        return spacer;
    }
    void set_spacer(unsigned v) {
        spacer = v;
    }

    void select(unsigned index, Framebuffer& fbuf) {
        // Invert icon
        icon_array[index].set_inverted(true);

        // Update self
        selected_index = index;

        // Update display
        show(fbuf);
    }

    void unselect(unsigned index, Framebuffer& fbuf) {
        // Un-invert icon
        icon_array[index].set_inverted(false);

        // Update self
        selected_index = -1;

        // Update display
        show(fbuf);
    }

    std::string_view get_selected_item() const {
        return icon_array[selected_index].get_name();
    }
};
#endif // ICON_HPP
