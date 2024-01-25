#include "icon.hpp"
#include "context.hpp"
#include "filesystem.hpp"



void Icon::set_inverted(bool v) {
    // Do nothing if no change
    if (v == inverted)
        return;

    // Invert buffer
    image.invert();

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

const Framebuffer& Toolbar::generate_data() {
    unsigned x = 0; //MAP: icon.py:169
    unsigned count = 0;

    // Blit icons into framebuffer
    for (const auto& icon : icon_array) {
        ++count;
        switch (icon.index()) {
        case 0: {
            const auto& icon_v = std::get<0>(icon).get_image();
            framebuf.blit(icon_v, x, 0);
            x += icon_v.get_width() + spacer;
        } break;
        case 1: {
            //TODO: Animation
        } break;
        }
    }

    // Return framebuffer
    return framebuf;
}
