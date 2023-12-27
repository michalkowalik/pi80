//
// Created by Michal Kowalik on 16.12.23.
//
#include <stdio.h>
#include "pico/stdlib.h"
#include "memory.h"
#include "../pins.h"
#include "../pio_handlers.h"


void set_memory_at(uint8_t address, uint8_t data) {
    printf("DEBUG: Writing to memory address 0x%02x, data 0x%02x\r\n", address, data);
    send_to_addressbus(address);
    printf("DEBUG: Address sent to address bus\r\n");
    send_to_databus(data);

    while ((BusPio->irq & 0x1) != 1) {
        sleep_us(10);
    }

    printf("DEBUG: Data sent to data bus\r\n");

    gpio_put(MREQ, 0); // enable
    gpio_put(WE, 0); // enable write
    sleep_us(10);
    gpio_put(WE, 1);
    gpio_put(MREQ, 1);

    printf("DEBUG: Memory write finished\r\n");
    // is_written = true;

    // wait for interrupt to be handled
    //while (is_written) {
    //    sleep_ms(1);
    // }

    printf("[IRQ] %lx - [ints0] %lx \r\n", BusPio->irq, BusPio->ints0);
    pio_interrupt_clear(BusPio, 0);
    // BusPio->irq = 0; // clear interrupt
    printf("[IRQ] %lx - [ints0] %lx \r\n", BusPio->irq, BusPio->ints0);
    printf("DEBUG: Memory write confirmed\r\n");
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