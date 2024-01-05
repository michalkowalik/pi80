#include <stdio.h>
#include "pico/stdlib.h"
#include "pio/pio_handlers.h"
#include "pins.h"
#include "memory/memory.h"
#include "boot_loader.h"
#include "slow_clock.h"

#define PIO_CLOCK_ENABLED true;

void init_pins() {

    // RST, WAIT, BUSREQ and INT are output-only, active low
    gpio_init(RST);
    gpio_pull_up(RST);
    gpio_set_dir(RST, GPIO_OUT);

    gpio_init(WAIT);
    gpio_pull_up(WAIT);
    gpio_set_dir(WAIT, GPIO_OUT);

    gpio_init(BUSREQ);
    gpio_pull_up(BUSREQ);
    gpio_set_dir(BUSREQ, GPIO_OUT);

    gpio_init(INT);
    gpio_pull_up(INT);
    gpio_set_dir(INT, GPIO_OUT);

    // MREQ is output-only on the Z80. Should be default input with pull-up on pico
    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_IN);

    // IOREQ is output-only on the Z80. Should be default input with pull-up on pico
    gpio_init(IOREQ);
    gpio_pull_up(IOREQ);
    gpio_set_dir(IOREQ, GPIO_IN);

    // BUSACK is input-only. Indicates, that Z80 has let another device control the bus
    gpio_init(BUSACK);
    gpio_pull_up(BUSACK);
    gpio_set_dir(BUSACK, GPIO_IN);

    // RD and WE are output-only on the Z80. Should be default input with pull-up on pico
    gpio_init(RD);
    gpio_pull_up(RD);
    gpio_set_dir(RD, GPIO_IN);

    gpio_init(WE);
    gpio_pull_up(WE);
    gpio_set_dir(WE, GPIO_IN);

    // User LED. no direct mapping to Z80
    gpio_init(LED);
    gpio_pull_up(LED);
    gpio_set_dir(LED, GPIO_OUT);
}


void load_stage1_bootloader() {
    printf("Loading Stage 1 bootloader.\r\n");
    uint16_t stage1_bootloader_length = sizeof(hello_worldSM) / sizeof(hello_worldSM[0]);
    printf("Stage 1 bootloader length: %d\r\n", stage1_bootloader_length);
    for (uint i = 0; i < stage1_bootloader_length; i++) {
        set_memory_at(i, hello_worldSM[i]);
    }
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
    zero_memory();
    load_stage1_bootloader();

    gpio_put(INT, 1);    // interrupt not active
    gpio_put(RST, 0);    // reset active
    gpio_put(BUSREQ, 0); // bus request active
    gpio_put(WAIT, 1);   // wait inactive

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#else
    gpio_init(CLK);
    gpio_set_dir(CLK, GPIO_OUT);
    slow_clock_init();
#endif

    // set address and databus as input with pull-up
    // (although that pull-up part is not clear to me)

    // set WR, RD and MREQ as inputs with pull-up
    gpio_set_dir(RD, GPIO_IN);
    gpio_set_dir(WE, GPIO_IN);
    gpio_set_dir(MREQ, GPIO_IN);

    sleep_ms(100);

    // release reset. Z80 should start executing code from address 0x0000
    printf("Stage 1 bootloader loaded. Releasing reset.\r\n");
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
                gpio_put(WAIT, 0);

                // set MEMREQ AND RD to OUTPUT
                gpio_set_dir(MREQ, GPIO_OUT);
                gpio_set_dir(RD, GPIO_OUT);
                gpio_put(MREQ, 0);
                gpio_put(RD, 0);

                dump_memory_to_stdout();

                gpio_put(RD, 1);
                gpio_put(MREQ, 1);

                // set MEMREQ AND RD to INPUT
                gpio_set_dir(MREQ, GPIO_IN);
                gpio_set_dir(RD, GPIO_IN);

                // release WAIT
                gpio_put(WAIT, 1);

                ch = -1;
            }
            ch = getchar_timeout_us(100);

        }
    }
}
