#include "event.hpp"



static inline uint32_t get_time_ms() {
    return us_to_ms(time_us_64());
}


void Timer::on_tick() {
    if (get_time_ms() >= expiry) {
        (*this)->set_active(false);
        on_done.set();
    }
}

basiccoro::AwaitableTask<void> Timer::async_sleep(unsigned int milliseconds) {
    background_sleep(milliseconds);
    co_await on_done;
}

void Timer::background_sleep(unsigned int milliseconds) {
    expiry = get_time_ms() + milliseconds;
    on_done.unset();
    (*this)->set_active(true);
}
