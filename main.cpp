#include "context.hpp"
#include "display.hpp"
#include "event.hpp"
#include "framebuffer.hpp"
#include "icon.hpp"
#include "basic-coro/AwaitableTask.hpp"

#include <vector>
#include <etl/array.h>
#include <etl/string.h>
#include <etl/to_string.h>
#include <etl/algorithm.h>
#include <pico/stdlib.h>



Context::Animations::Animations(AsyncMan& aman) :
    poopy(aman, "poop", Animation::bounce, 96, 48, 16, 16),
    baby(aman, "baby_bounce", Animation::bounce, 48, 16, 48, 48),
    eat(aman, "eat", Animation::default_, 48, 16, 48, 48),
    babyzzz(aman, "baby_zzz", Animation::loop, 48, 16, 48, 48),
    death(aman, "skull", Animation::loop, 54, 32, 16, 16),
    go_potty(aman, "potty", Animation::default_, 64, 16, 48, 48),
    call_animate(aman, "call_animate", Animation::default_, 108, 0, 16, 16) {
    death.speed = Animation::slow;
    baby.speed = Animation::slow;
    babyzzz.speed = Animation::very_slow;
    call_animate.speed = Animation::very_slow;
    call_animate.set_pause_when_done(true);
    go_potty.repeats = 1;

    poopy->active = false;
    baby->active = false;
    eat->active = false;
    babyzzz->active = false;
    death->active = false;
    go_potty->active = false;
}


Context::Context() : tb(async_man, {
                           Image("food.pbm", 16, 16, "food"),
                           Image("lightbulb.pbm", 16, 16, "lightbulb"),
                           Image("game.pbm", 16, 16, "game"),
                           Image("firstaid.pbm", 16, 16, "firstaid"),
                           Image("toilet.pbm", 16, 16, "toilet"),
                           Image("heart.pbm", 16, 16, "heart"),
                           Image("call.pbm", 16, 16, "call")
                        }),
    animations(async_man), chibi(animations.baby), fbuf(oled.size.width, oled.size.height) {}


void Context::popup(etl::string_view message, Image image) {
    constexpr auto size = Display::size;

    fbuf.clear();
    fbuf.rect(0, size.width-1, 16, size.height-1);
    fbuf.blit(image.get_framebuffer(), 5, 26);
    oled.begin_frame();
    oled.render_framebuffer(fbuf);
    oled.render_text({28, 30}, message);
    oled.end_frame();

    sleep_ms(2000);
}

/// Just poops, then goes back
basiccoro::AwaitableTask<void> Context::poop() {
    // Back up previous animation
    AsyncObject& previous = chibi;
    // Play pooping animatin
    chibi = animations.go_potty;
    co_await animations.go_potty.wait_done();
    // Show poop
    animations.poopy->active = true;
    // Restore previous animation
    chibi = previous;
}
/// Poops every now and then
basiccoro::AwaitableTask<void> Context::poop_loop() {
    Timer timer(async_man);

    while (true) {
        // Wait about 7 minutes
        co_await timer.async_sleep(420500);
        // Decrease happiness if poop hasn't been cleaned up
        if (animations.poopy->active)
            happiness *= 0.7f;
        // Poop again
        co_await poop();
        // Increase happiness
        happiness = std::min(happiness+0.1f, 0.0f);
    }
}

/// Implements sleep cycle
basiccoro::AwaitableTask<void> Context::energy_loop() {
    Timer timer(async_man);

    while (true) {
        // Wait 7.5 seconds
        co_await timer.async_sleep(7500);
        // Check if sleeping
        if (animations.babyzzz->active) {
            // Increase energy, health and happiness if possible
            health = etl::min(health + 0.011f, 1.0f);
            happiness = etl::min(happiness*1.03f + 0.1f, 1.1f) - 0.1f;
            energy += 0.025f/health;
            // Stop sleeping if full energy
            if (energy >= 1.0f) {
                energy = 1.0f;
                chibi = animations.baby;
            }
        } else {
            // Decrease energy if possible, health and happiness too if close to depletion
            energy -= 0.025f/health;
            if (energy < 0.2f)
                happiness = etl::clamp(happiness*0.82f, 0.0f, 0.7f);
            if (energy < 0.1f)
                health = etl::max(health*0.9f, 0.0f);
            // Fall asleep if too tired
            if (energy <= 0.01f) {
                energy = 0.0f;
                chibi = animations.babyzzz;
            }
        }
    }
}

/// Decreases health if hygiene is bad
basiccoro::AwaitableTask<void> Context::hygiene_loop() {
    Timer timer(async_man);

    while (true) {
        // Wait 15 seconds
        co_await timer.async_sleep(15000);

        // Decrease health if there's poop
        if (animations.poopy->active)
            health = std::max(health * 0.95f, 0.0f);
    }
}

void Context::run() {
    etl::array<char, Display::size.buffer_size()> fbuf_data;
    if (!fbuf.load(fbuf_data))
        panic("Bad main fbuf");

    // Start loops
    poop_loop();
    energy_loop();
    hygiene_loop();

    // Main loop
    while (true) { //MAP: picotamachibi.py:94
        // Clear framebuffer
        fbuf.clear();

        printf("health = %f  energy = %f  happiness = %f\n", health, energy, happiness);

        // Check for death
        if (health < 0.01f)
            chibi = animations.death;

        // Process input
        if (button_dbg.was_pushed())
            health = 0.f;
        if (button_a.was_pushed())
            tb.previous();
        else if (button_b.was_pushed())
            tb.next();

        // Tick everything
        async_man.tick();

        // Render complete image
        oled.fullframe_framebuffer(fbuf);

        // Just wait
        sleep_ms(50); //MAP: picotamachibi.py:213
    }
}


int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    std::vector<etl::byte> context_buf(sizeof(Context));

    Context::create(context_buf);
    auto& ctx = Context::get();
    ctx.run();
    Context::destroy();
}
