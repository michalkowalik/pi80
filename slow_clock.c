//
// Created by Michal Kowalik on 01.01.24.
//

#include <hardware/gpio.h>
#include <pico/time.h>
#include "pins.h"

bool clock_state = false;
struct repeating_timer clock_timer_id;

void toggle_clock() {
    gpio_put(CLK, 1);
    sleep_ms(200);
    gpio_put(CLK, 0);
}

bool clock_timer(struct repeating_timer *t) {
    gpio_put(CLK, clock_state);
    clock_state = !clock_state;
    return true;
}

void slow_clock_init() {
    add_repeating_timer_ms(10, clock_timer, NULL, &clock_timer_id);
}