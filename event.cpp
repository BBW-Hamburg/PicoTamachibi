#include "event.hpp"
#include "framebuffer.hpp"
#include "display.hpp"

#include <pico/stdlib.h>



void Event::popup(Framebuffer& fbuf, Display& oled) {
    constexpr auto size = Display::size;

    fbuf.clear();
    fbuf.rect(0, size.width-1, 16, size.height-1);
    fbuf.blit(sprite.get_image(), 5, 26);
    oled.begin_frame();
    oled.render_framebuffer(fbuf);
    oled.render_text({28, 30}, message);
    oled.end_frame();

    sleep_ms(2000);
}
