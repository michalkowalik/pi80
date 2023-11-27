#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_clock.h"

#define PIO_CLOCK_ENABLED true;

int main() {
    stdio_init_all();
    sleep_ms(1000);                // give the board (and Minicom) some time to connect to the serial interface
    printf("Booting Pi80..\r\n");

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#endif

    while (true) {
        tight_loop_contents();
    }
}
