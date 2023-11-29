#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_init.h"
#include "pins.h"

#define PIO_CLOCK_ENABLED true;

void init_pins() {
    gpio_init(RST);
    gpio_pull_up(RST);
    gpio_set_dir(RST, GPIO_OUT);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);                // give the board (and Minicom) some time to connect to the serial interface
    printf("Booting Pi80..\r\n");

    // initialize CPU
    init_pins();
    gpio_put(RST, 0);      // RESET the CPU

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#endif
    init_databus();


    // send halt => should turn the LED on
    send_to_databus(0x76);
    sleep_ms(10);    // needed?

    // release reset
    gpio_put(RST, 1);



    while (true) {
        tight_loop_contents();
    }
}
