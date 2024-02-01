#include "context.hpp"

#include <etl/string_view.h>
#include <etl/array.h>
#include <new>

static Context *instance = nullptr;


void Context::create(etl::span<etl::byte> buffer) {
    instance = reinterpret_cast<Context*>(buffer.data());
    new (instance) Context;
}

void Context::destroy() {
    delete (instance);
}

Context &Context::get() {
    return *instance;
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
