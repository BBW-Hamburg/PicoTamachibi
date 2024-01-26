#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define GPIO_IN 0
#define GPIO_IRQ_EDGE_RISE 0
#define GPIO_FUNC_I2C 0

#ifdef __cplusplus
extern "C" {
#endif

void gpio_set_irq_enabled_with_callback_t(unsigned gpio, uint32_t event_mask);

void gpio_init(unsigned gpio);
void gpio_deinit(unsigned gpio);
bool gpio_get(unsigned gpio);
void gpio_set_irq_enabled_with_callback(unsigned gpio, unsigned, bool, typeof(gpio_set_irq_enabled_with_callback_t) handler);

inline static void gpio_set_function(unsigned gpio, unsigned) {}
inline static void gpio_set_dir(unsigned gpio, unsigned) {}
inline static void gpio_pull_down(unsigned gpio) {}
inline static void gpio_pull_up(unsigned gpio) {}


inline static uint64_t time_us_64() {
    struct timeval te;
    gettimeofday(&te, NULL);
    uint64_t microeconds = ((uint64_t)te.tv_sec)*1000000LL + te.tv_usec;
    return microeconds;
}

inline static void sleep_ms(uint32_t ms) {
    usleep(((uint64_t)ms) * 1000);
}

inline static uint32_t us_to_ms(uint64_t v) {
    return v / 1000;
}

#ifdef __cplusplus
}
#endif
