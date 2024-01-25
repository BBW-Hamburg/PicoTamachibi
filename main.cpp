#include "context.hpp"

#include "display.hpp"
#include "framebuffer.hpp"
#include "icon.hpp"

#include <vector>
#include <pico/stdlib.h>



void Context::run() {
    Framebuffer fb(display.size.width, display.size.height);
    std::vector<char> fb_buf(fb.get_buffer_size());
    if (!fb.load(fb_buf))
        panic("Init buf fail");

    unsigned y = 0;
    while (true) {
        // Create test frame
        Icon icon("heart.pbm");
        fb.blit(icon.get_image(), 10, y);
        if (++y == display.size.height)
            y = 0;
        display.fullframe_framebuffer(fb);

        // Delay
        sleep_ms(50); //MAP: picotamachibi.py:213
    }
}


int main() {
    Context::create();
    auto& ctx = Context::get();
    ctx.run();
}
