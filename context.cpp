#include "context.hpp"

#include <etl/string_view.h>
#include <etl/array.h>
#include <new>


static char context_buf[sizeof(Context)] = {};

void Context::create() {
    new (reinterpret_cast<Context*>(context_buf)) Context;
}

Context &Context::get() {
    return *reinterpret_cast<Context*>(context_buf);
}

void Context::panic(etl::string_view message) {
    etl::array<etl::string_view, 3> lines = {
        "Panic:",
        message,
        "!FAIL FAST!"
    };

    oled.fullframe_text(lines);

    while (true) asm("nop");
}
