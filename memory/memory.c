//
// Created by Michal Kowalik on 16.12.23.
//
#include <stdio.h>
#include "pico/stdlib.h"
#include "memory.h"
#include "../pins.h"
#include "../pio/pio_handlers.h"


void set_memory_at(uint8_t address, uint8_t data) {
    gpio_set_dir(MREQ, GPIO_OUT);
    gpio_set_dir(WE, GPIO_OUT);

    send_to_addressbus(address);
    while ((AddressPio->irq & 0x2) != 2) {
        sleep_us(10);
    }

    send_to_databus(data);
    while ((BusPio->irq & 0x1) != 1) {
        sleep_us(10);
    }

    gpio_put(MREQ, 0); // enable
    gpio_put(WE, 0); // enable write
    sleep_us(10);
    gpio_put(WE, 1);
    gpio_put(MREQ, 1);

    pio_interrupt_clear(BusPio, DataBusIRQ);
    pio_interrupt_clear(AddressPio, AddressBusIRQ);

    gpio_set_dir(MREQ, GPIO_IN);
    gpio_set_dir(WE, GPIO_IN);
}

void dump_memory_to_stdout() {
    printf("DEBUG: Dumping memory to stdout");

    // send wait to cpu
    gpio_put(WAIT, 0);

    // set MEMREQ AND RD to OUTPUT
    gpio_set_dir(MREQ, GPIO_OUT);
    gpio_set_dir(RD, GPIO_OUT);
    gpio_put(MREQ, 0);
    gpio_put(RD, 0);


    gpio_set_dir(MREQ, GPIO_OUT);
    gpio_set_dir(RD, GPIO_OUT);

    for (uint addr = 0; addr <= 0xff; addr++) {
        send_to_addressbus(addr);
        while ((AddressPio->irq & 0x2) != 2) {
            sleep_us(10);
        }

        gpio_put(MREQ, 0);
        gpio_put(RD, 0); // enable read
        sleep_us(10);           // not sure if needed, but won't hurt
        uint32_t memory_cell = read_from_databus();
        gpio_put(RD, 1);
        gpio_put(MREQ, 1);
        pio_interrupt_clear(AddressPio, 1); // clear interrupt
        if (addr % 16 == 0) {
            printf("\r\n%04x:  ", addr);
        }
        printf("%02lx  ", memory_cell);
    }
    printf("\r\n");

    gpio_set_dir(MREQ, GPIO_IN);
    gpio_set_dir(RD, GPIO_IN);

    gpio_put(RD, 1);
    gpio_put(MREQ, 1);

    // set MEMREQ AND RD to INPUT
    gpio_set_dir(MREQ, GPIO_IN);
    gpio_set_dir(RD, GPIO_IN);

    // release WAIT
    gpio_put(WAIT, 1);

}

void test_memory() {
    printf("Write to memory addresses 0x00 - 0xff:\r\n");
    for(uint addr = 0; addr <= 0xff; addr++) {
        set_memory_at(addr, addr);   // write NOP to memory
        if (addr % 16 == 0) {
            printf("\r\n%04x:  ", addr);
        }
        printf("%02x,  ", addr);
    }

    set_memory_at(0x00, 0x76);

    printf("\r\nConfirm write...\r\n");
    printf("Read from addresses 0x00 - 0xff:\r\n");

    dump_memory_to_stdout();
    printf("\r\n");
}

void zero_memory() {
    for(uint addr = 0; addr <= 0xff; addr++) {
        set_memory_at(addr, 0x00);
    }
}