#include "context.hpp"

#include <string_view>
#include <array>
#include <new>


static char context_buf[sizeof(Context)] = {};

void Context::create() {
    new (reinterpret_cast<Context*>(context_buf)) Context;
}

Context &Context::get() {
    return *reinterpret_cast<Context*>(context_buf);
}

void Context::panic(std::string_view message) {
    std::array<std::string_view, 3> lines = {
        "Panic:",
        message,
        "!FAIL FAST!"
    };

    oled.fullframe_text(lines);

    while (true) asm("nop");
}
