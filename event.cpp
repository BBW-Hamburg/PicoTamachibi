#include "event.hpp"
#include "context.hpp"
#include "framebuffer.hpp"
#include "display.hpp"

#include <etl/vector.h>
#include <pico/stdlib.h>



void Event::popup(Display& oled) {
    constexpr auto size = Display::size;
    Framebuffer fbuf(size.width, size.height); //MAP: icon.py:598
    etl::array<char, size.buffer_size()> fbuf_data;
    if (!fbuf.load(fbuf_data))
        Context::get().panic("Popup bad buf");


    fbuf.rect(0, size.width-1, 16, size.height-1);
    fbuf.blit(sprite.get_image(), 5, 26);
    oled.begin_frame();
    oled.render_framebuffer(fbuf);
    oled.render_text({28, 30}, message);
    oled.end_frame();

    sleep_ms(2000);
}

void Event::tick() {
    // Increment timer and return if not expired
    if (++timer_elapsed < timer)
        return;

    // Get rid of timer
    unset_timer();
}
