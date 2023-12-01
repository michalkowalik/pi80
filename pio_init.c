//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pins.h"

#include "squarewave.pio.h"
#include "databus.pio.h"

#define CLK_FREQ 4000

PIO databus_pio;
uint databus_sm;

void start_clock() {
    printf("DEBUG: starting the pio clock\r\n");
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &squarewave_program);

    float div = (float) clock_get_hz(clk_sys) / CLK_FREQ;

    squarewave_program_init(pio, sm, offset, div, CLK);
    pio_sm_set_enabled(pio, sm, true);
}

void init_databus() {
    printf("DEBUG: Starting PIO SM for the databus\r\n");
    databus_pio = pio1;
    uint offset = pio_add_program(databus_pio, &databus_program);
    databus_sm = pio_claim_unused_sm(databus_pio, true);
    databus_program_init(databus_pio, databus_sm, offset, 0, 8);
}

void send_to_databus(uint8_t data) {
    pio_sm_put_blocking(databus_pio, databus_sm, data);
}
