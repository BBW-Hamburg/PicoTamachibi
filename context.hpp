#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#include "human_input.hpp"
#include "display.hpp"
#include "filesystem.hpp"
#include "icon.hpp"
#include "asyncman.hpp"
#include "basic-coro/AwaitableTask.hpp"

#include <etl/string_view.h>
#include <etl/span.h>
#include <hardware/i2c.h>


class Context {
public:
    // Pubic hardware
    Display oled = Display(sda, scl, i2c0);

    const Button button_a = 4, //MAP: picotamachibi.py:63
                 button_b = 3,
                 button_x = 2,
                 button_dbg = 8;

    Filesystem filesystem;

    // Public state
    AsyncMan async_man;
    Framebuffer fbuf;

private:
    Context();

    // Hardware
    static constexpr unsigned sda = 0, //MAP: picotamachibi.py:9
                              scl = 1;

    // State
    float health = 1.0f,
          happiness = 0.5f,
          energy = 1.0f;

    // Animations
    struct Animations {
        Animations(AsyncMan& aman);

        Animation poopy, baby, eat, babyzzz, death, go_potty, call_animate;
    } animations;

    AsyncSelector chibi;

    // Toolbar
    Toolbar tb;

    // Helper functions
    void popup(etl::string_view, Image);

    // Coroutines
    basiccoro::AwaitableTask<void> poop();
    basiccoro::AwaitableTask<void> poop_loop();
    basiccoro::AwaitableTask<void> energy_loop();
    basiccoro::AwaitableTask<void> hygiene_loop();

public:
    // Prohibit copy and move
    Context(Context&&) = delete;
    Context(const Context&) = delete;

    // Helper functions
    void run();

    [[noreturn]]
    void panic(etl::string_view message);

    static void create(etl::span<etl::byte>);
    static void destroy();
    static Context& get();
};


#define ASSERT_PANIC(message, ...) if (!(__VA_ARGS__)) ::Context::get().panic(message)
#endif // CONTEXT_HPP
