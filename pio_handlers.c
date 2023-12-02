//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pins.h"

#include "squarewave.pio.h"
#include "databus.pio.h"
#include "addressbus.pio.h"

#define CLK_FREQ 4000

PIO buses_pio = pio1;
uint databus_sm;
uint addressbus_sm;

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
    uint offset = pio_add_program(buses_pio, &databus_program);
    databus_sm = pio_claim_unused_sm(buses_pio, true);
    databus_program_init(buses_pio, databus_sm, offset, D0, DATA_BUS_WIDTH);
}

void init_addressbus() {
    printf("DEBUG: Starting PIO SM for the addressbus\r\n");
    uint offset = pio_add_program(buses_pio, &addressbus_program);
    addressbus_sm = pio_claim_unused_sm(buses_pio, true);
    databus_program_init(buses_pio, addressbus_sm, offset, A0, ADDRESS_BUS_WIDTH);
    pio_sm_set_enabled(buses_pio, addressbus_sm, true);
}

void send_to_databus(uint8_t data) {
    pio_sm_put_blocking(buses_pio, databus_sm, data);
}

void send_to_addressbus(uint8_t address) {
    pio_sm_put_blocking(buses_pio, addressbus_sm, address);
}