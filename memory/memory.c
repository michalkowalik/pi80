//
// Created by Michal Kowalik on 16.12.23.
//
#include <stdio.h>
#include "pico/stdlib.h"
#include "memory.h"
#include "../pins.h"
#include "../pio_handlers.h"


void set_memory_at(uint8_t address, uint8_t data) {
    send_to_addressbus(address);
    send_to_databus(data);

    gpio_put(MREQ, 0); // enable
    gpio_put(WE, 0); // enable write
    sleep_us(10);
    gpio_put(WE, 1);
    gpio_put(MREQ, 1);
}

void dump_memory_to_stdout() {
    printf("DEBUG: Dumping memory to stdout");
    for (uint addr = 0; addr <= 0xff; addr++) {
        send_to_addressbus(addr);
        sleep_us(100);
        gpio_put(MREQ, 0);
        gpio_put(RD, 0); // enable read
        sleep_us(10);           // not sure if needed, but won't hurt
        uint32_t memory_cell = get_from_databus();
        gpio_put(RD, 1);
        gpio_put(MREQ, 1);
        if (addr % 16 == 0) {
            printf("\r\n");
        }
        printf("%02x: %02lx, ", addr, memory_cell);
    }
    printf("\r\n");
}