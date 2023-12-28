#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_handlers.h"
#include "pins.h"
#include "memory/memory.h"

#define PIO_CLOCK_ENABLED true;

void init_pins() {
    gpio_init(RST);
    gpio_pull_up(RST);
    gpio_set_dir(RST, GPIO_OUT);

    gpio_init(WAIT);
    gpio_pull_up(WAIT);
    gpio_set_dir(WAIT, GPIO_OUT);

    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_OUT);

    gpio_init(INT);
    gpio_pull_up(INT);
    gpio_set_dir(INT, GPIO_OUT);

    gpio_init(IOREQ);
    gpio_pull_up(IOREQ);
    gpio_set_dir(IOREQ, GPIO_IN);

    gpio_init(RD);
    gpio_pull_up(RD);
    gpio_set_dir(RD, GPIO_OUT);

    gpio_init(WE);
    gpio_pull_up(WE);
    gpio_set_dir(WE, GPIO_OUT);

    gpio_init(LED);
    gpio_pull_up(LED); // needed?
    gpio_set_dir(LED, GPIO_OUT);
}

void test_memory() {
    printf("Write to memory addresses 0x00 - 0xff:\r\n");
    for(uint addr = 0; addr <= 0xff; addr++) {
        set_memory_at(addr, addr);   // write NOP to memory
        if (addr % 16 == 0) {
            printf("\r\n");
        }
        printf("%02x: %02x, ", addr, addr);
    }

    set_memory_at(0x00, 0x76);

    printf("\r\nConfirm write...\r\n");
    printf("Read from addresses 0x00 - 0xff:\r\n");

    dump_memory_to_stdout();
    printf("\r\n");
}


int main() {
    stdio_init_all();
    sleep_ms(1500);                // give the board (and Minicom) some time to connect to the serial interface
    printf("Booting Pi80..\r\n");
    printf("Press CTRL+[ to cycle the clock\r\n");
    printf("Press CTRL+] to dump memory\r\n");

    // initialize CPU
    init_pins();

    init_databus();
    init_addressbus();

    test_memory();

    gpio_put(INT, 1);    // interrupt not active
    gpio_put(RST, 0);    // reset active
    gpio_put(BUSREQ, 0); // bus request active
    gpio_put(WAIT, 1);   // wait inactive

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#endif

    // set address and databus as input with pull-up
    // (although that pull-up part is not clear to me)

    // set WR, RD and MREQ as inputs with pull-up
    gpio_set_dir(RD, GPIO_IN);
    gpio_set_dir(WE, GPIO_IN);
    gpio_set_dir(MREQ, GPIO_IN);


    // release reset. Z80 should start executing code from address 0x0000
    gpio_put(BUSREQ, 1);
    gpio_put(RST, 1);


    while (true) {
        int16_t ch = getchar_timeout_us(100);
        while (ch != PICO_ERROR_TIMEOUT) {
            if (0x1b == ch) {
                printf("cycling clock\r\n");
                // trigger interrupt ?
                ch = -1;
            }

            if (0x1d == ch) {
                // send wait to cpu
                // set MEMREQ AND RD to OUTPUT
                dump_memory_to_stdout();
                // set MEMREQ AND RD to INPUT
                // release WAIT

                ch = -1;
            }
            ch = getchar_timeout_us(100);

        }
    }
}
