#ifndef ICON_HPP
#define ICON_HPP
#include "framebuffer.hpp"
#include "asyncman.hpp"

#include <etl/string_view.h>
#include <etl/vector.h>
#include <etl/variant.h>
#include <etl/hash.h>


class Image {
    friend class Animation;

    Framebuffer image;
    bool inverted = false;
    size_t name_hash;

public:
    Image(const char *filename, unsigned width = 16, unsigned height = 16, etl::string_view name = "Empty")
          : image(width, height) {
        name_hash = etl::hash<etl::string_view>()(name);
        set_framebuffer(load(filename));
    }

    bool operator ==(etl::string_view v) const {
        return etl::hash<etl::string_view>()(v) == name_hash;
    }

    const Framebuffer& get_framebuffer() const {
        return image;
    }
    void set_framebuffer(const Framebuffer::ROData& buf);

    bool is_inverted() const {
        return inverted;
    }
    void set_inverted(bool v);

    Framebuffer::ROData load(const char *filename);
};


class Animation {
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
        reverse,
        bounce
    };

private:
    etl::vector<Image, 16> frames;
    AnimationSpeed speed = normal;
    AnimationType type = default_;
    UniqueAsyncManHandle async;

    unsigned frame_index,
             step = 0;
    unsigned short repeats = -1;
    unsigned x,
             y;

    void load(const char *filename, unsigned width, unsigned height);
    void update_frame_index();

public:
    Animation(AsyncMan& aman, const char *filename = NULL, AnimationType animation_type = default_, unsigned x = 0, unsigned y = 0, unsigned width = 16, unsigned height = 16, etl::vector<Image, 16>&& frames = {});
    Animation(const Animation&) = delete;
    Animation(Animation&&) = delete;

    AnimationSpeed get_speed() const {
        return speed;
    }
    void set_speed(AnimationSpeed v) {
        speed = v;
    }

    AnimationType get_type() const {
        return type;
    }
    void set_type(AnimationType v) {
        type = v;
        reset();
    }

    unsigned get_frame_count() const {
        return frames.size();
    }

    bool is_done() const {
       if (type == default_)
            return frame_index >= frames.size();
        return repeats < frame_index/frames.size();
    }

    void reset() {
        frame_index = 0;
    }
    void set_active(bool v) {
        async->active = v;
    }
    bool is_active() const {
        return async->active;
    }

    unsigned short get_repeats() const {
        return repeats;
    }
    void set_repeats(unsigned short v) {
        repeats = v;
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

    const Image& get_current_image() const {
        return frames[frame_index];
    }
};


using OptionallyAnimatedIconBase = etl::variant<Image, Animation>;

class OptionallyAnimatedIcon : public OptionallyAnimatedIconBase {
public:
    using OptionallyAnimatedIconBase::OptionallyAnimatedIconBase;

    bool operator ==(etl::string_view) const;
    void set_inverted(bool v);
};


class Toolbar {
    etl::vector<OptionallyAnimatedIcon, 14> images;
    constexpr static unsigned spacer = 2;
    int selection_index;

public:
    Toolbar(decltype(images)&& images, unsigned initial_index = 0)
          : images(std::move(images)), selection_index(initial_index) {
        this->images[selection_index].set_inverted(true);
    }

    Toolbar(const Toolbar&) = delete;

    Toolbar(Toolbar&& o)
        : images(etl::move(o.images)), selection_index(o.selection_index) {}

    bool operator ==(etl::string_view v) const {
        return images[selection_index] == v;
    }

    void set_selection_index(unsigned new_index) {
        images[selection_index].set_inverted(false);
        images[new_index].set_inverted(true);

        selection_index = new_index;
    }

    void show(Framebuffer& fbuf);
};
#endif // ICON_HPP
