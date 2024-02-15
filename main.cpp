#include "context.hpp"
#include "display.hpp"
#include "framebuffer.hpp"
#include "icon.hpp"

#include <etl/array.h>
#include <etl/string.h>
#include <etl/to_string.h>
#include <pico/stdlib.h>



Context::Animations::Animations(AsyncMan& aman) :
    poopy(aman, "poop", Animation::bounce, 96, 48, 16, 16),
    baby(aman, "baby_bounce", Animation::bounce, 48, 16, 48, 48),
    eat(aman, "eat", Animation::default_, 48, 16, 48, 48),
    babyzzz(aman, "baby_zzz", Animation::loop, 48, 16, 48, 48),
    death(aman, "skull", Animation::loop, 0, 16, 16, 16),
    go_potty(aman, "potty", Animation::default_, 64, 16, 48, 48),
    call_animate(aman, "call_animate", Animation::default_, 108, 0, 16, 16) {
    death.set_speed(Animation::slow);
    babyzzz.set_speed(Animation::very_slow);
    call_animate.set_speed(Animation::very_slow);
    call_animate.set_pause_when_done(true);
    go_potty.set_repeats(1);
    go_potty->active = true;
    poopy->active = false;
}


Context::Context() : tb({
                           Image("food.pbm", 16, 16, "food"),
                           Image("lightbulb.pbm", 16, 16, "lightbulb"),
                           Image("game.pbm", 16, 16, "game"),
                           Image("firstaid.pbm", 16, 16, "firstaid"),
                           Image("toilet.pbm", 16, 16, "toilet"),
                           Image("heart.pbm", 16, 16, "heart"),
                           Image("call.pbm", 16, 16, "call")
                        }),
    animations(async_man), fbuf(oled.size.width, oled.size.height) {}


void Context::run() {
    etl::array<char, Display::size.buffer_size()> fbuf_data;
    if (!fbuf.load(fbuf_data))
        panic("Bad main fbuf");

    // Main loop
    while (true) { //MAP: picotamachibi.py:94
        // Clear framebuffer
        fbuf.clear();

        //Logic...

        // Tick everything
        async_man.tick();

        // Render complete image
        oled.fullframe_framebuffer(fbuf);
        sleep_ms(50); //MAP: picotamachibi.py:213
    }
}


int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    etl::array<etl::byte, sizeof(Context)> context_buf;

    Context::create(context_buf);
    auto& ctx = Context::get();
    ctx.run();
    Context::destroy();
}
