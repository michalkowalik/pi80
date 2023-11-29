//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include "hardware/pio.h"
#include "pins.h"

#include "squarewave.pio.h"
#include "databus.pio.h"

#define CLK_DIVIDER 30000 // clock divider = 120MHz / 30 = 4MHz

PIO databus_pio;
uint databus_sm;

void start_clock() {
    printf("DEBUG: starting the pio clock\r\n");
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &squarewave_program);
    uint sm = pio_claim_unused_sm(pio, true);
    squarewave_program_init(pio, sm, offset, CLK_DIVIDER, CLK);
}

void init_databus() {
    printf("DEBUG: Starting PIO SM for the databus");
    databus_pio = pio1;
    uint offset = pio_add_program(databus_pio, &databus_program);
    databus_sm = pio_claim_unused_sm(databus_pio, true);
    databus_program_init(databus_pio, databus_sm, offset, 0, 8);
}

void send_to_databus(uint8_t data) {
    pio_sm_put_blocking(databus_pio, databus_sm, data);
}
