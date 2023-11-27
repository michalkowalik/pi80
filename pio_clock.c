//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include "hardware/pio.h"

#include "squarewave.pio.h"

#define CLK_PIN 18     // clock pin
#define CLK_DIVIDER 30 // clock divider = 120MHz / 30 = 4MHz

void start_clock() {
    printf("starting the pio clock\r\n");
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &squarewave_program);
    uint sm = pio_claim_unused_sm(pio, true);
    squarewave_program_init(pio, sm, offset, CLK_DIVIDER, CLK_PIN);
}