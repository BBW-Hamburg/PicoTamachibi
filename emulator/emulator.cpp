#include "emulator.h"

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <MiniFB.h>



namespace Emulator {
struct Button {
    std::function<void (unsigned id, uint32_t)> callback = nullptr;
    bool is_pushed = false;
};

static std::map<int, Button> buttons;

static void mfb_keyboard_callback(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool is_pressed) {
    const static std::map<mfb_key, int> mapping = {
        {KB_KEY_0, 0},
        {KB_KEY_1, 1},
        {KB_KEY_2, 2},
        {KB_KEY_3, 3},
        {KB_KEY_4, 4},
        {KB_KEY_5, 5},
        {KB_KEY_6, 6},
        {KB_KEY_7, 7},
        {KB_KEY_8, 8},
        {KB_KEY_9, 9},
    };

    // Find id in map
    int id;
    {
        auto res = mapping.find(key);
        if (res == mapping.end())
            return;
        id = res->second;
    }

    // Find button according to map
    Button *btn;
    {
        auto res = buttons.find(id);
        if (res == buttons.end())
            return;
        btn = &res->second;
    }

    // Update button
    btn->is_pushed = is_pressed;

    // Callback as needed if any
    if (is_pressed && btn->callback)
        btn->callback(id, 0);
}


class Display {
    mfb_window *window = nullptr;
    std::vector<uint32_t> buffer;

    const unsigned width, height;

    uint32_t& pixel(unsigned x, unsigned y) {
        return buffer[y * width + x];
    }

public:
    Display(unsigned width, unsigned height)
          : width(width), height(height) {
        constexpr unsigned scale = 6;
        window = mfb_open_ex("Pico Emulator", width*scale, height*scale, 0);
        mfb_set_keyboard_callback(window, mfb_keyboard_callback);
        buffer.resize(width*height);
    }
    ~Display() {
        mfb_close(window);
    }

    bool check() const {
        return window && !buffer.empty();
    }

    void set_pixel(unsigned x, unsigned y, bool v) {
        pixel(x, y) = v?0xffffff:0x000000;
    }

    void clear() {
        std::fill(buffer.begin(), buffer.end(), 0x000000);
    }

    void show() {
        mfb_update_ex(window, buffer.data(), width, height);
        mfb_wait_sync(window);
    }
} *display = nullptr;
}


extern "C" {
bool emu_display_enable(unsigned width, unsigned height) {
    if (Emulator::display)
        delete Emulator::display;
    Emulator::display = new Emulator::Display(width, height);
    return Emulator::display->check();
}

void emu_display_disable() {
    if (Emulator::display)
        delete Emulator::display;
}

void emu_display_clear() {
    Emulator::display->clear();
}

void emu_display_set_pixel(unsigned x, unsigned y, bool v) {
    Emulator::display->set_pixel(x, y, v);
}

void emu_display_show() {
    Emulator::display->show();
}


void emu_button_add(int id) {
    Emulator::buttons.emplace(id, Emulator::Button());
}

bool emu_button_is_held(int id) {
    return Emulator::buttons.at(id).is_pushed;
}

void emu_button_register_callback(int id, typeof(gpio_set_irq_enabled_with_callback_t) *callback) {
    Emulator::buttons.at(id).callback = callback;
}

bool emu_button_remove(int id) {
    return Emulator::buttons.erase(id);
}
}
