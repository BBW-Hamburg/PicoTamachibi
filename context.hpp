#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#include "human_input.hpp"
#include "display.hpp"
#include "filesystem.hpp"
#include "icon.hpp"
#include "event.hpp"

#include <etl/string_view.h>
#include <etl/span.h>
#include <hardware/i2c.h>


class Context {
public:
    // Pubic hardware
    Display oled = Display(sda, scl, i2c0);

    const Button button_a = 4, //MAP: picotamachibi.py:63
        button_b = 3,
        button_x = 2;

    Filesystem filesystem;

private:
    // Hardware
    static constexpr unsigned sda = 0, //MAP: picotamachibi.py:9
                              scl = 1;

    // State
    unsigned health = 1, //MAP: picotamachibi.py:18
             happiness = 1,
             energy = 1;

    // Icons
    struct Icons {
        Icons();

        Icon food, lightbulb, game, firstaid, toilet, heart, call;
        Animate poopy, baby, eat, babyzzz, death, go_potty, call_animate;
    } icons;

    // Events
    struct Events {
        Event energy_increase, firstaid, toilet, sleep_time, heart_status;

        Events(Icons&);
    } events;

    Toolbar tb;

    Context();

    // Private functions
    void build_toolbar();

public:
    // Prohibit copy and move
    Context(Context&&) = delete;
    Context(const Context&) = delete;

    void run();

    [[noreturn]]
    void panic(etl::string_view message);

    static void create(std::span<std::byte>);
    static void destroy();
    static Context& get();
};


#define ASSERT_PANIC(message, ...) if (!(__VA_ARGS__)) ::Context::get().panic(message)
#endif // CONTEXT_HPP
