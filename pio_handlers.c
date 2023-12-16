//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include "pio_handlers.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pins.h"

#include "databus.pio.h"
#include "addressbus.pio.h"
#include "clock.pio.h"

#define CLK_FREQ 4000

PIO buses_pio = pio1;
uint databus_sm;
uint addressbus_sm;

uint8_t value;


void start_clock() {
    printf("DEBUG: starting the pio clock\r\n");
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &clock_program);

    float div = (float) clock_get_hz(clk_sys) / CLK_FREQ;

    clock_program_init(pio, sm, offset, div, CLK);
    pio_sm_set_enabled(pio, sm, true);
}

/* just in case..
void __time_critical_func(bus_read_handler)() {
    uint32_t data = pio_sm_get_blocking(buses_pio, databus_sm);
    value_read = true;
    value = data & 0xff;
    pio_interrupt_clear(buses_pio, PIO0_IRQ_0);
}
*/

void init_databus() {
    printf("DEBUG: Starting PIO SM for the databus\r\n");
    uint offset = pio_add_program(buses_pio, &databus_program);
    databus_sm = pio_claim_unused_sm(buses_pio, true);
    databus_program_init(buses_pio, databus_sm, offset, D0, DATA_BUS_WIDTH);
    pio_sm_set_enabled(buses_pio, databus_sm, true);
}

void init_addressbus() {
    printf("DEBUG: Starting PIO SM for the addressbus\r\n");
    uint offset = pio_add_program(buses_pio, &addressbus_program);
    addressbus_sm = pio_claim_unused_sm(buses_pio, true);
    databus_program_init(buses_pio, addressbus_sm, offset, A0, ADDRESS_BUS_WIDTH);
    pio_sm_set_enabled(buses_pio, addressbus_sm, true);
}

/*
 * - Send 9 bytes: 8 bytes of data
 *                 1 byte with value 1 to indicate Write
 * - Trigger Interrupt PIO0
 */
void send_to_databus(uint32_t data) {
    uint32_t data_to_send = (data << 1) | 1;
    pio_sm_put_blocking(buses_pio, databus_sm, data_to_send);
}

/*
 * - Send 1 byte -> 0 to indicate the read
 * - trigger interrupt
 * - read from PIO's ISR
 */
uint32_t get_from_databus() {
    pio_sm_put_blocking(buses_pio, databus_sm, 2); // non-zero, but LSB = 0
    return pio_sm_get_blocking(buses_pio, databus_sm) >> 24;
}

void send_to_addressbus(uint8_t address) {
    pio_sm_put_blocking(buses_pio, addressbus_sm, address);
 }
