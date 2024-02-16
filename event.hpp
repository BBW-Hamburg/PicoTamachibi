#ifndef EVENT_HPP
#define EVENT_HPP
#include "asyncman.hpp"
#include "basic-coro/AwaitableTask.hpp"
#include "basic-coro/SingleEvent.hpp"

#include <etl/string.h>
#include <etl/string_view.h>
#include <pico/stdlib.h>


class Timer final : public AsyncObject {
    basiccoro::SingleEvent<void> on_done;
    unsigned expiry;

    void on_tick() override;

public:
    Timer(AsyncMan& aman) : AsyncObject(aman, false) {}

    basiccoro::AwaitableTask<void> async_sleep(unsigned milliseconds);
    void background_sleep(unsigned milliseconds);

    bool has_expired() const {
        return on_done.isSet();
    }
};

#endif // EVENT_HPP
