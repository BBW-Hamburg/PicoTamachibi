#include "event.hpp"
#include "framebuffer.hpp"
#include "display.hpp"

#include <vector>
#include <pico/stdlib.h>



void Event::popup(Display& oled) {
    constexpr auto size = Display::size;
    Framebuffer fbuf(size.width, size.height); //MAP: icon.py:598
    std::vector<char> fbuf_data(fbuf.get_buffer_size());

    fbuf.rect(0, size.width, 16, 64);
    fbuf.blit(sprite.get_image(), 5, 26);
    fbuf.text(message, 32, 34);
    oled.fullframe_framebuffer(fbuf);

    sleep_ms(2000);
}

void Event::tick() {
    // Increment timer and return if not expired
    if (++timer_elapsed < timer)
        return;

    // Do callback if possible
    if (callback)
        callback();

    // Get rid of timer
    unset_timer();
}
