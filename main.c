#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_handlers.h"
#include "pins.h"

#define PIO_CLOCK_ENABLED true;

void init_pins() {
    gpio_init(RST);
    gpio_pull_up(RST);
    gpio_set_dir(RST, GPIO_OUT);

    gpio_init(WAIT);
    gpio_set_dir(WAIT, GPIO_OUT);

    gpio_init(INT);
    gpio_pull_up(INT);
    gpio_set_dir(INT, GPIO_OUT);

    gpio_init(BUSACK);
    gpio_set_dir(BUSACK, GPIO_IN);

    gpio_init(BUSREQ);
    gpio_pull_up(BUSREQ);
    gpio_set_dir(BUSREQ, GPIO_OUT);

    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_IN);

    gpio_init(IOREQ);
    gpio_pull_up(IOREQ);
    gpio_set_dir(IOREQ, GPIO_IN);


    gpio_init(RD);
    gpio_pull_up(RD);
    gpio_set_dir(RD, GPIO_IN);

    gpio_init(WR);
    gpio_pull_up(WR);
    gpio_set_dir(WR, GPIO_IN);

}

int main() {
    stdio_init_all();
    sleep_ms(2000);                // give the board (and Minicom) some time to connect to the serial interface
    printf("Booting Pi80..\r\n");


    // initialize CPU
    init_pins();
    gpio_put(INT, 1);      // Interrupt NOT Active
    gpio_put(RST, 0);      // RESET the CPU
    gpio_put(BUSREQ, 1);
    gpio_put(WAIT, 1);

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#endif
    init_databus();
    init_addressbus();

    // send halt => should turn the LED on
    send_to_databus(0x76);

    // release reset
    gpio_put(RST, 1);
    sleep_ms(1);


    while (true) {
        int16_t ch = getchar_timeout_us(100);
        while (ch != PICO_ERROR_TIMEOUT) {
            printf("got %d\r\n", ch);
            ch = getchar_timeout_us(100);
        }
        //tight_loop_contents();
    }
}
