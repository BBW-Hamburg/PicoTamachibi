#include "context.hpp"
#include "display.hpp"
#include "framebuffer.hpp"
#include "icon.hpp"

#include <etl/array.h>
#include <etl/string.h>
#include <etl/to_string.h>
#include <pico/stdlib.h>



Context::Icons::Icons() :
    food("food.pbm", 16, 16, 0, 0, "food"), //MAP: picotamachibi.py:23
    lightbulb("lightbulb.pbm", 16, 16, 0, 0, "lightbulb"),
    game("game.pbm", 16, 16, 0, 0, "game"),
    firstaid("firstaid.pbm", 16, 16, 0, 0, "firstaid"),
    toilet("toilet.pbm", 16, 16, 0, 0, "toilet"),
    heart("heart.pbm", 16, 16, 0, 0, "heart"),
    call("call.pbm", 16, 16, 0, 0, "call"),
    poopy("poop", Animate::default_, 96,48, 16, 16),
    baby("baby_bounce", Animate::bounce, 48,16, 48, 48),
    eat("eat", Animate::default_, 48,16, 48, 48),
    babyzzz("baby_zzz", Animate::loop, 48,16, 48, 48),
    death("skull", Animate::bounce, 0,16, 16, 16),
    go_potty("potty", Animate::bounce, 64,16, 48, 48),
    call_animate("call_animate", Animate::default_, 108, 0, 16, 16) {
    baby.do_bounce(); //MAP: picotamachibi.py:84
    poopy.do_bounce();
    death.do_loop();
    death.set_speed(Animate::slow);
    death.set_active(true);
    babyzzz.set_speed(Animate::very_slow);
    call_animate.set_speed(Animate::very_slow);
    go_potty.do_loop(1);
    go_potty.set_active(true);
    poopy.set_active(false);
}


Context::Events::Events(Icons& icons) :
    energy_increase(icons.heart, "Increase Energy", 1),
    firstaid(icons.firstaid, "First Aid", 0),
    toilet(icons.toilet, "Toilet", 0),
    sleep_time(icons.lightbulb, "sleep time", 1),
    heart_status(icons.heart, "Status", 0) {}


Context::Context() : events(icons) {
    build_toolbar();
}

void Context::build_toolbar() {
    tb.set_spacer(2); //MAP: picotamachibi.py:43
    tb.add_item(icons.food);
    tb.add_item(icons.lightbulb);
    tb.add_item(icons.game);
    tb.add_item(icons.firstaid);
    tb.add_item(icons.toilet);
    tb.add_item(icons.heart);
    tb.add_item(icons.call);
}

void Context::run() {
    Framebuffer fbuf(oled.size.width, oled.size.height);
    etl::array<char, Display::size.width*Display::size.height/8> fbuf_data;
    if (!fbuf.load(fbuf_data))
        panic("Bad main fbuf");
    fbuf.clear();

    unsigned index = 0; //MAP: picotamachibi.py:67
    tb.select(index, fbuf);
    bool cancel = false,
         feeding_time = false,
         sleeping = false;

    // Main loop
    while (true) { //MAP: picotamachibi.py:94
        if (!cancel)
            tb.unselect(index, fbuf);
        if (button_a.was_pushed()) {
            index += 1;
            if (index == 7)
                index = 0;
            cancel = false;
        }
        if (button_x.was_pushed()) {
            cancel = true;
            index = -1;
        }

        if (!cancel)
            tb.select(index, fbuf);

        if (button_b.was_pushed()) {
            if (tb.get_selected_item() == "food") {
                feeding_time = true;
                sleeping = false;
                icons.baby.unload();
            }

            if (tb.get_selected_item() == "toilet") {
                events.toilet.set_message("Cleaning...");
                events.toilet.popup(oled);
                icons.poopy.set_active(false);
                icons.baby.set_active(true);
                happiness += 1;
                fbuf.clear();
                icons.poopy.unload();
            }
            if (tb.get_selected_item() == "lightbulb") {
                if (!sleeping) {
                    sleeping = true;
                    icons.babyzzz.load();
                    events.sleep_time.set_message("Night Night");
                    events.sleep_time.popup(oled);
                    fbuf.clear();
                    // need to add an event that increases energy level after sleeping for 1 minute
                } else {
                    sleeping = false;
                    icons.babyzzz.unload();
                }
            }
            if (tb.get_selected_item() == "firstaid") {
                events.firstaid.set_message("Vitamins");
                events.firstaid.popup(oled);
                health += 1;

                fbuf.clear();
            }
            if (tb.get_selected_item() == "heart") {
                etl::string<12> health_msg = "health = ";
                etl::to_string(health, health_msg, true);
                etl::string<12> happy_msg = "happy = ";
                etl::to_string(happiness, happy_msg, true);
                etl::string<12> energy_msg = "energy = ";
                etl::to_string(energy, energy_msg, true);
                events.heart_status.set_message(health_msg);
                events.heart_status.popup(oled);
                events.heart_status.set_message(happy_msg);
                events.heart_status.popup(oled);
                events.heart_status.set_message(energy_msg);
                events.heart_status.popup(oled);
                fbuf.clear();
            }
            if (tb.get_selected_item() == "call") {
                // call_animate.animate(oled)
                icons.call_animate.set_active(false);
            }
        }

        // Time for Poop?
        // poop_check()
        // poop_event.tick()

        if (feeding_time) {
            icons.eat.load();
            if (!icons.eat.is_done())
                icons.eat.do_animate(fbuf);
            if (feeding_time && icons.eat.is_done()) {
                feeding_time = false;
                events.energy_increase.set_message("ENERGY + 1");
                events.energy_increase.popup(oled);
                energy += 1;

                fbuf.clear();
                icons.eat.unload();
                icons.baby.load();
            }
        } else {
            if (sleeping) {
                icons.babyzzz.do_animate(fbuf);
            } else {
                if (icons.baby.is_active())
                    icons.baby.do_animate(fbuf);
                if (icons.go_potty.is_active())
                    icons.go_potty.do_animate(fbuf);
                if (icons.go_potty.is_done()) {
                    icons.go_potty.set_active(false);
                    icons.poopy.set_active(true);
                    icons.baby.load();
                    icons.baby.do_bounce();
                    icons.baby.set_active(true);
                }
            }
        }
        if ((energy <= 1) && (happiness <= 1) && (health <= 1))
            icons.death.set_active(true);
        else
            icons.death.set_active(false);

        if ((energy <= 1) || (happiness <= 1) || (health <= 1))
            // set the toolbar call icon to flash
            icons.call_animate.set_active(true);
        else
            icons.call_animate.set_active(false);

        if (icons.poopy.is_active()) {
            icons.poopy.load();
            icons.poopy.do_animate(fbuf);
        }
        if (icons.death.is_active())
            icons.death.do_animate(fbuf);
        tb.show(fbuf);
        if (index == 6)
            tb.select(index, fbuf);
        else
            if (icons.call_animate.is_active())
                icons.call_animate.do_animate(fbuf);

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
