#include "icon.hpp"
#include "context.hpp"
#include "filesystem.hpp"

#include <cstdio>


void Image::set_framebuffer(const Framebuffer::ROData &buf) {
    if (!image.load_ro(buf))
        Context::get().panic("Fail Img load");
}

void Image::set_inverted(bool v) {
    // Do nothing if no change
    if (v == inverted)
        return;

    // Invert buffer
    image.inverted = v;

    // Update boolean
    inverted = v;
}

Framebuffer::ROData Image::load(const char *filename) {
    auto fres = Context::get().filesystem.read_file(filename); //MAP: icon.py:139

    // Skip first 3 lines
    unsigned off = 0;
    while (fres[++off] != '\n');
    while (fres[++off] != '\n');
    while (fres[++off] != '\n');
    ++off;

    // Return subspan
    return fres.subspan(off);
}


void Animation::load(const char *filename, unsigned width, unsigned height) {
    printf("Loading animation files: %s\n", filename);
    for (const auto& file : Context::get().filesystem) {
        etl::string_view this_filename = file.name;
        if (this_filename.starts_with(filename) && this_filename.ends_with(".pbm")) {
            printf(" - %s\n", file.name);
            frames.push_back(Image(file.name, width, height, this_filename));
        }
    }
    printf("Done loading animation files.\n\n");
}

void Animation::update_frame_index() {
    // Get real_step according to speed
    unsigned real_step;
    switch (speed) {
    case very_slow: real_step = step/20; break;
    case slow: real_step = step/2; break;
    case normal: real_step = step; break;
    case fast: real_step = step*2; break;
    }

    // Update frame_index according to animation type
    switch (type) {
    case default_: frame_index = real_step; break;
    case loop: frame_index = real_step % frames.size();
    case reverse: frame_index = frames.size() - 1 - (real_step % frames.size());
    case bounce: {
        const auto logical_frame = real_step % (frames.size()*2);
        if (logical_frame < frames.size())
            frame_index = logical_frame;
        else
            frame_index = frames.size() - (logical_frame - frames.size());
    } break;
    default: Context::get().panic("Bd ImgT");
    }

    frame_index = frame_index % frames.size();
}

Animation::Animation(AsyncMan &aman, const char *filename, AnimationType animation_type, unsigned int x, unsigned int y, unsigned int width, unsigned int height, etl::vector<Image, 16> &&frames)
      : frames(etl::move(frames)), type(animation_type), async(aman), x(x), y(y) {
    load(filename, width, height);
    update_frame_index();

    async->on_tick = [this, ctx = &Context::get()] {
        if (!is_done()) {
            ctx->fbuf.blit(get_current_image().get_framebuffer(), this->x, this->y);
            ++step;
            update_frame_index();
        }
    };
}


bool OptionallyAnimatedIcon::operator ==(etl::string_view v) const {
    if (index() == 0) {
        return etl::get<0>(*this) == v;
    }
    Context::get().panic("SInv missing");
}

void OptionallyAnimatedIcon::set_inverted(bool v) {
    if (index() == 0) {
        return etl::get<0>(*this).set_inverted(v);
    }
    Context::get().panic("SInv missing");
}


void Toolbar::show(Framebuffer& fbuf) {
    unsigned x = 0; //MAP: icon.py:169
    unsigned count = 0;

    // Blit icons into framebuffer
    for (const auto& icon : images) {
        ++count;
        switch (icon.index()) {
        case 0: {
            const auto& icon_v = etl::get<0>(icon).get_framebuffer();
            fbuf.blit(icon_v, x, 0);
            x += icon_v.get_width() + spacer;
        } break;
        case 1: {
            const auto& animate_v = etl::get<1>(icon);
            fbuf.blit(animate_v.get_current_image().get_framebuffer(), x, 0);
        } break;
        }
    }
}
