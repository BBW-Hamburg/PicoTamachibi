#include "icon.hpp"
#include "context.hpp"
#include "filesystem.hpp"

#include <cstdio>


void Icon::set_image(const Framebuffer::ROData &buf) {
    if (!image.load_ro(buf))
        Context::get().panic("Fail Img load");
}

void Icon::set_inverted(bool v) {
    // Do nothing if no change
    if (v == inverted)
        return;

    // Invert buffer
    image.inverted = v;

    // Update boolean
    inverted = v;
}

Framebuffer::ROData Icon::load_icon(const char *filename) {
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


void Animate::set_active(bool v) {
    if (active == v)
        return;
    active = v;
    if (v)
        load();
    else
        unload();
}

void Animate::set_speed(AnimationSpeed v) {
    speed = v;
    switch (speed) {
    case very_slow: {
        pause = 10;
        speed_value = 10;
    } break;
    case slow: {
        pause = 1;
        speed_value = 1;
    } break;
    case normal: {
        pause = 0;
        speed_value = 0;
    } break;
    case fast: {}
    }
}

void Animate::do_forward() {
    if (speed == normal) //MAP: icon.py:315
        current_frame += 1;

    if (speed <= slow) {
        if (pause > 0) {
            pause -= 1;
        } else {
            current_frame += 1;
            pause = speed_value;
        }
    }

    if (speed == fast) {
        if (current_frame < get_frame_count() + 2) {
            current_frame += 2;
        } else {
            current_frame += 1;
        }
    }
}

void Animate::do_reverse() {
    if (speed == normal) //MAP: icon.py:332
        current_frame -= 1;

    if (speed <= slow) {
        if (pause > 0) {
            pause -= 1;
        } else {
            current_frame -= 1;
            pause = speed_value;
        }
    }

    if (speed == fast) {
        if (current_frame < get_frame_count() + 2)
            current_frame -= 2;
        else
            current_frame -= 1;
    }
}

void Animate::load() {
    if (cached)
        return;

    printf("Loading animation files: %s\n", filename);
    for (const auto& file : Context::get().filesystem) {
        std::string_view this_filename = file.name;
        if (this_filename.starts_with(filename) && this_filename.ends_with(".pbm")) {
            printf(" - %s\n", file.name);
            frames.push_back(Icon(file.name, width, height, 0, 0, this_filename));
        }
    }
    printf("Done loading animation files.\n\n");

    cached = true;

    frames.shrink_to_fit();
}

void Animate::unload() {
    frames.clear();
    cached = false;
}

void Animate::do_animate(Framebuffer &fbuf) {
    ASSERT_PANIC("Ani not load", cached && frames.size() != 0);

    const auto cf = current_frame; //MAP: icon.py:371
    const auto& frame = frames[cf];
    fbuf.blit(frame.image, x, y);

    if (animation_type == loop) {
        // Loop from the first frame to the last, for the number of cycles specificed, and then set done to True
        do_forward();

        if (current_frame > get_frame_count()) {
            current_frame = 0;
            loop_count -= 1;
            if (loop_count == 0)
                done = true;
        }
    }

    if (animation_type == bouncing) {
        // Loop from the first frame to the last, and then back to the first again, then set done to True
        if (bouncing) {
            if (current_frame == 0) {
                if (loop_count == 0) {
                    done == true; // Wrong operator, but will leave as is to avoid breaking anything
                } else if (loop_count > 0) {
                    loop_count -= 1;
                    do_forward();
                    bouncing = false;
                }
                if (loop_count == -1) {
                    // bounce infinately
                    do_forward();
                    bouncing = false;
                }
            }
            if ((current_frame < get_frame_count()) && (current_frame > 0))
                do_reverse();
        } else {
            if (current_frame == 0) {
                if (loop_count == 0) {
                    done == true; // Wrong operator, but will leave as is to avoid breaking anything
                } else if (loop_count == -1) {
                    // bounce infinatey
                    do_forward();
                } else {
                    do_forward();
                    loop_count -= 1;
                }
            } else if (current_frame == get_frame_count()) {
                do_reverse();
                bouncing = true;
            } else {
                do_forward();
            }
        }
    }

    if (animation_type == default_) {
        // loop through from first frame to last, then set done to True
        if (current_frame == get_frame_count()) {
            current_frame = 0;
            done = true;
        } else {
            do_forward();
        }
    }
}


void OptionallyAnimatedIcon::set_inverted(bool v) {
    if (index() == 0) {
        return std::get<0>(*this).set_inverted(v);
    }
    Context::get().panic("SInv missing");
}

std::string_view OptionallyAnimatedIcon::get_name() const {
    if (index() == 0) {
        return std::get<0>(*this).get_name();
    }
    Context::get().panic("GName missing");
}


void Toolbar::show(Framebuffer& fbuf) {
    unsigned x = 0; //MAP: icon.py:169
    unsigned count = 0;

    // Blit icons into framebuffer
    for (const auto& icon : icon_array) {
        ++count;
        switch (icon.index()) {
        case 0: {
            const auto& icon_v = std::get<0>(icon).get_image();
            fbuf.blit(icon_v, x, 0);
            x += icon_v.get_width() + spacer;
        } break;
        case 1: {
            // Were blitting manually even though Animate has code for this purpose.......
            const auto& animate_v = std::get<1>(icon);
            fbuf.blit(animate_v.frames[animate_v.current_frame].get_image(), x, 0);
        } break;
        }
    }
}
