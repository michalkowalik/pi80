#include <stdio.h>
#include "pico/stdlib.h"
#include "pio/pio_handlers.h"
#include "pins.h"
#include "memory/memory.h"
#include "boot_loader.h"
#include "slow_clock.h"

#define PIO_CLOCK_ENABLED true;

bool debug = false;
bool debug2 = false;
int uart_char = 0;

void init_pins() {

    // RST, BUSREQ and INT are output-only, active low
    gpio_init(RST);
    gpio_pull_up(RST);
    gpio_set_dir(RST, GPIO_OUT);

    gpio_init(BUSREQ);
    gpio_pull_up(BUSREQ);
    gpio_set_dir(BUSREQ, GPIO_OUT);

    gpio_init(INT);
    gpio_pull_up(INT);
    gpio_set_dir(INT, GPIO_OUT);

    // WAIT is triggered by the flip-flop. Should be default input with pull-up on pico
    gpio_init(WAIT);
    gpio_pull_up(WAIT);
    gpio_set_dir(WAIT, GPIO_IN);


    // MREQ is output-only on the Z80. Should be default input with pull-up on pico
    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_IN);

    // WAIT_RES flips the flip-flop and releases the WAIT state.
    gpio_init(WAIT_RES);
    gpio_pull_up(WAIT_RES);
    gpio_set_dir(WAIT_RES, GPIO_OUT);

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
    uint16_t stage1_bootloader_length = sizeof(hello_world_interactive) / sizeof(hello_world_interactive[0]);
    printf("Stage 1 bootloader length: %d\r\n", stage1_bootloader_length);
    for (uint i = 0; i < stage1_bootloader_length; i++) {
        set_memory_at(i, hello_world_interactive[i]);
    }
}

void uart_callback(void *context) {
    assert(context != NULL);
    int *i = (int *) context;
    *i = 1;
}

void read_from_uart(uint8_t  *ch) {
    if (uart_char != 0) {
        int c;
        while (c = getchar_timeout_us(0), c != -1) {
            *ch = c;
        }
        uart_char = 0;
    }
}

int main() {

    stdio_init_all();

    // give the board (and Minicom) some time to connect to the serial interface
    sleep_ms(1500);

    // set callback for serial input
    stdio_set_chars_available_callback(uart_callback, &uart_char);

    printf("Booting Pi80..\r\n");
    printf("Press CTRL+[ to cycle the clock\r\n");
    printf("Press CTRL+] to dump memory\r\n");

    // initialize CPU
    init_pins();

    init_databus();
    init_addressbus();

    // test_memory();
    zero_memory();
    load_stage1_bootloader();
    dump_memory_to_stdout();

    gpio_put(INT, 1);    // interrupt not active
    gpio_put(RST, 0);    // reset active
    gpio_put(BUSREQ, 0); // bus request active
    gpio_put(WAIT, 1);   // wait inactive
    gpio_put(WAIT_RES, 0); // wait_res low to reset the flip-flop

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


    // release reset. Z80 should start executing code from address 0x0000
    printf("Stage 1 bootloader loaded. Releasing reset.\r\n\r\n");
    sleep_ms(1000);
    gpio_put(WAIT_RES, 1);
    gpio_put(BUSREQ, 1);
    gpio_put(RST, 1);

    while (true) {
        // IO operation requested
        if (gpio_get(WAIT) == 0) {
            // Write operation requested
            if (gpio_get(WE) == 0) {
                if (debug) printf("DEBUG: Write operation requested\r\n");

                // read Z80's address bus
                uint32_t io_address = read_from_addressbus();
                uint8_t io_data = read_from_databus() & 0xff;
                if (debug) printf("DEBUG: IO address: %02lx, IO Data: %02x\r\n", io_address, io_data);

                switch (io_address) {
                    case 0x00:
                        printf("DEBUG: IO Address 0x00. No operation implemented\r\n");
                        break;
                    case 0x01:
                        if (debug) printf("Serial TX requested\r\n");
                        printf("%c", io_data);
                        break;
                    default:
                        printf("DEBUG: Unknown or not implemented IO address\r\n");
                }

                // control bus sequence to exit from a wait state
                gpio_put(BUSREQ, 0);         // Request for a DMA
                gpio_put(WAIT_RES, 0);       // Reset WAIT flip-flop exiting from the wait state
                sleep_us(10);                // TODO: 10us might be too much. Keep an eye on this
                gpio_put(WAIT_RES, 1);       // now the Z80 is in DMA, so it's safe to set wait_res to 1
                gpio_put(BUSREQ, 1);         // resume normal operation


                // Read operation requested
            } else if (gpio_get(RD) == 0) {
                if (debug2) printf("DEBUG: Read operation requested\r\n");

                uint32_t io_address = read_from_addressbus();
                uint8_t  io_data = 0x00;

                switch (io_address) {
                    case 0x00:
                        printf("DEBUG: IO Address 0x00. No operation implemented\r\n");
                        break;
                    case 0x01:
                        // NOTE 1: if there is no input char, a value 0xFF is forced as input char.
                        // NOTE 2: the INT_ signal is always reset (set to HIGH) after this I/O operation.

                        if (debug2) printf("Serial RX requested\r\n");

                        io_data = 0xff;
                        read_from_uart(&io_data);
                        gpio_put(INT, 1);
                        break;
                    case 0x02:
                        // read boot phase 2 payload

                        break;
                    default:
                        printf("DEBUG: Unknown or not implemented IO address\r\n");
                }

                // send io_data to the databus
                send_to_databus(io_data);
                while ((BusPio->irq & 0x1) != 1) {
                    sleep_us(1);
                }
                gpio_put(BUSREQ, 0);                       // Request for a DMA
                gpio_put(WAIT_RES, 0);                     // Reset WAIT flip-flop exiting from the wait state
                sleep_us(10);                              // TODO: 10us might be too much. Keep an eye on this
                pio_interrupt_clear(BusPio, DataBusIRQ);
                gpio_put(WAIT_RES, 1);                     // now the Z80 is in DMA, so it's safe to set wait_res to 1
                gpio_put(BUSREQ, 1);                       // resume normal operation


            } else {
                if (debug) printf("DEBUG: Interrupt requested?\r\n");
            }
        }
    }
}
