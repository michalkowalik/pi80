#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_handlers.h"
#include "pins.h"


void init_pins() {
    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_OUT);

    gpio_init(RD);
    gpio_pull_up(RD);
    gpio_set_dir(RD, GPIO_OUT);

    gpio_init(WE);
    gpio_pull_up(WE);
    gpio_set_dir(WE, GPIO_OUT);

}


void test_memory() {
    printf("Write to memory addresses 0x00 - 0xff:");
    for(uint addr = 0; addr <= 0xff; addr++) {
        send_to_addressbus(addr);
        send_to_databus(addr);

        gpio_put(MREQ, 0); // enable
        gpio_put(WE, 0); // enable write
        sleep_us(10);
        gpio_put(WE, 1);
        gpio_put(MREQ, 1);
        if (addr % 16 == 0) {
            printf("\r\n");
        }
        printf("%02x: %02x, ", addr, addr);
    }


    send_to_addressbus(0);
    send_to_databus(0);
    gpio_put(MREQ, 0); // enable
    gpio_put(WE, 0); // enable write
    sleep_us(10);
    gpio_put(WE, 1);
    gpio_put(MREQ, 1);

    printf("\r\nConfirm write...\r\n");
    printf("Read from addresses 0x00 - 0xff:\r\n");


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
        printf("%02x: %02x, ", addr, memory_cell);
    }

    printf("\r\n");
}


int main() {
    stdio_init_all();
    sleep_ms(1500);                // give the board (and Minicom) some time to connect to the serial interface
    printf("Booting Pi80..\r\n");

    // initialize CPU
    init_pins();

    init_databus();
    init_addressbus();

    test_memory();

    // send halt => should turn the LED on
    send_to_databus(0x76);

    while (true) {
        int16_t ch = getchar_timeout_us(100);
        while (ch != PICO_ERROR_TIMEOUT) {
            printf("got 0x%02x\r\n", ch);
            ch = getchar_timeout_us(100);
        }
        //tight_loop_contents();
    }
}
