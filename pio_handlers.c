//
// Created by Michal Kowalik on 27.11.23.
//
#include <stdio.h>
#include <pico/time.h>
#include "pio_handlers.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pins.h"

#include "databus.pio.h"
#include "addressbus.pio.h"
#include "clock.pio.h"

PIO ClockPio = pio1;
PIO AddressPio = pio1;
PIO BusPio = pio0;
// bool is_written = false;

void start_clock() {
    printf("DEBUG: starting the pio clock\r\n");
    PIO pio = ClockPio;
    uint offset = pio_add_program(pio, &clock_program);

    float div = (float) clock_get_hz(clk_sys) / CLK_FREQ;
    printf("clock div: %f\r\n", div);

    clock_program_init(pio, ClockSM, offset, div, CLK);
    pio_sm_set_enabled(pio, ClockSM, true);
}

void __time_critical_func(databus_write_handler)() {
    gpio_put(LED, 1);
    printf("DEBUG: [IRQ] %lx - \r\n", BusPio->irq);
    //while (!is_written) {
    //    sleep_ms(1);
   // }
    printf("DEBUG: clearing interrupt\r\n");
    pio_interrupt_clear(BusPio, WriteIRQ);
    printf("DEBUG: interrupt cleared\r\n");
    is_written = false;
    gpio_put(LED, 0);
    printf("debug: is_written set to false\r\n");
}

void init_databus() {
    printf("DEBUG: Starting PIO SM for the databus\r\n");
    uint offset = pio_add_program(BusPio, &databus_program);
    databus_program_init(BusPio, DataBusSM, offset, D0, DATA_BUS_WIDTH);

    // set IRQ source
    pio_set_irq0_source_enabled(BusPio, pis_interrupt0, true);
    irq_set_priority(WriteIRQ, PICO_HIGHEST_IRQ_PRIORITY);
    //irq_set_exclusive_handler(WriteIRQ, databus_write_handler);
    //irq_set_enabled(WriteIRQ, true);

    pio_sm_set_enabled(BusPio, DataBusSM, true);
}

void init_addressbus() {
    printf("DEBUG: Starting PIO SM for the addressbus\r\n");
    uint offset = pio_add_program(AddressPio, &addressbus_program);
    addressbus_program_init(AddressPio, AddressBusSM, offset, A0, ADDRESS_BUS_WIDTH);
    pio_sm_set_enabled(AddressPio, AddressBusSM, true);
}

/*
 * - Send 9 bytes: 8 bytes of data
 *                 1 byte with value 1 to indicate Write
 * - Trigger Interrupt PIO0
 */
void send_to_databus(uint32_t data) {
    uint32_t data_to_send = (data << 1) | 1;
    pio_sm_put_blocking(BusPio, DataBusSM, data_to_send);
}

/*
 * - Send 1 byte -> 0 to indicate the read
 * - trigger interrupt
 * - read from PIO's ISR
 */
uint32_t get_from_databus() {
    pio_sm_put_blocking(BusPio, DataBusSM, 2); // non-zero, but LSB = 0
    return pio_sm_get_blocking(BusPio, DataBusSM) >> 24;
}

void send_to_addressbus(uint8_t address) {
    pio_sm_put_blocking(AddressPio, AddressBusSM, address);
 }
