#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pio/pio_handlers.h"

#include "io_operations.h"
#include "pins.h"
#include "memory/memory.h"
#include "boot_loader.h"
#include "rom_data/basic.h"
#include "rom_data/forth.h"
#include "rom_data/cpm.h"
#include "slow_clock.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "LoopDoesntUseConditionVariableInspection"
#define PIO_CLOCK_ENABLED true;

bool debug = true;
bool debug2 = false;
bool Z80_interrupt_flag = false;
char uart_char = '\0';
uint8_t *stage2;
uint16_t stage2_size;
uint index_stage2 = 0;      // index for stage 2 bootloader


void handle_io_write();
void handle_io_read();

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

    // WAIT_RES flips the flip-flop and releases the WAIT state.
    gpio_init(WAIT_RES);
    gpio_pull_up(WAIT_RES);
    gpio_set_dir(WAIT_RES, GPIO_OUT);

    // BUSACK is input-only. Indicates, that Z80 has let another device control the bus
    gpio_init(BUSACK);
    gpio_pull_up(BUSACK);
    gpio_set_dir(BUSACK, GPIO_IN);

    // MEMRQ, RD and WE are output-only on the Z80. Should be default input with pull-up on pico
    // we set them as input later, after the bootloader is loaded
    gpio_init(RD);
    gpio_set_dir(RD, GPIO_OUT);

    gpio_init(WE);
    gpio_pull_up(WE);
    gpio_set_dir(WE, GPIO_OUT);

    gpio_init(MREQ);
    gpio_pull_up(MREQ);
    gpio_set_dir(MREQ, GPIO_OUT);


    // User LED. no direct mapping to Z80
    gpio_init(LED);
    gpio_pull_up(LED);
    gpio_set_dir(LED, GPIO_OUT);
}


void load_stage1_bootloader() {
    uart_printf("Loading Stage 1 bootloader.\r\n");
    uint16_t stage1_bootloader_length = sizeof(boot_stage1) / sizeof(boot_stage1[0]);
    uart_printf("Stage 1 bootloader length: %d bytes.\r\n", stage1_bootloader_length);
    for (uint i = 0; i < stage1_bootloader_length; i++) {
        set_memory_at(i, boot_stage1[i]);
    }
}

/*
 * Handle data coming from Piper
 * see pi80per's repo for the protocol description
 * anything coming here is a command: it has a header and a payload
 */
void uart0_irq_handler() {
    uint8_t command;

    while (uart_is_readable(UART_ID)) {
        command = uart_getc(UART_ID);
        if (command == 1) {
            uart_char = uart_getc(UART_ID);
            if (Z80_interrupt_flag)
                gpio_put(INT, 0);                // trigger interrupt to Z80
        }
        else if (command == 4 || command == 3 || command == 2 || command == 6) { // floppy operation confirmation
            uint8_t floppy_status = uart_getc(UART_ID);
            if (debug)
                printf("DEBUG: Floppy operation %d confirmation received: %d\r\n", command, floppy_status);
            floppy_operation_complete = true;
        }
        else if (command == 7) {             // read floppy sector
            uint8_t sector_len = uart_getc(UART_ID);
            uint8_t i = 0;
            while (i < sector_len)
                *(sector_buffer + i++) = uart_getc(UART_ID);
            floppy_operation_complete = true;
        } else {
            printf("Unknown Command received from UART: %02x\r\n", command);
        }

    }
}

void pi_uart_init() {
    // setup UART
    uart_init(UART_ID, BAUD_RATE);
    uart_set_fifo_enabled(UART_ID, true);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);

    irq_set_exclusive_handler(UART0_IRQ, uart0_irq_handler);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);

}

void initialize_pi80() {
    uint8_t boot_choice = 0;

    stdio_init_all();
    stdio_usb_init();
    pi_uart_init();
    sleep_ms(3000);
    uart_printf("Starting Pi80.\r\n");

    sleep_ms(5);
    uart_printf("Select boot mode:\r\n");
    sleep_ms(5);
    uart_printf("  1: BASIC\r\n");
    sleep_ms(5);
    uart_printf("  2: Forth\r\n");
    sleep_ms(5);
    uart_printf("  3: CP/M from DISK0\r\n");
    sleep_ms(5);
    uart_printf("> ");

    uart_char = '\0';
    while(uart_char == '\0' || uart_char == 255) {
        sleep_ms(100);
    }
    boot_choice = uart_char;
    uart_char = '\0';
    uart_printf("%c\r\n", boot_choice);

    // modify the starting address of the stage 2 bootloader
    // modify the length of the stage 2 bootloader
    switch (boot_choice) {
        case '2':
            boot_stage1[2] = stage2_forth_start_address & 0xff;
            boot_stage1[3] = (stage2_forth_start_address >> 8) & 0xff;
            boot_stage1[4] = stage2_forth_size & 0xff;
            boot_stage1[5] = (stage2_forth_size >> 8) & 0xff;
            stage2 = stage2_forth;
            stage2_size = stage2_forth_size;
            break;
        case '3':
            boot_stage1[2] = stage2_cpm_start_address & 0xff;
            boot_stage1[3] = (stage2_cpm_start_address >> 8) & 0xff;
            boot_stage1[4] = stage2_cpm_loader_size & 0xff;
            boot_stage1[5] = (stage2_cpm_loader_size >> 8) & 0xff;
            stage2 = stage2_cpm_loader;
            stage2_size = stage2_cpm_loader_size;
            break;
        default:
            boot_stage1[2] = stage2_basic_start_address & 0xff;
            boot_stage1[3] = (stage2_basic_start_address >> 8) & 0xff;
            boot_stage1[4] = stage2_basic_size & 0xff;
            boot_stage1[5] = (stage2_basic_size >> 8) & 0xff;
            stage2 = stage2_basic;
            stage2_size = stage2_basic_size;
            Z80_interrupt_flag = true;
            break;
    }

    // initialize CPU
    init_pins();

    gpio_put(INT, 1);    // interrupt not active
    gpio_put(RST, 0);    // reset active
    gpio_put(BUSREQ, 0); // bus request active
    gpio_put(WAIT, 1);   // wait inactive
    gpio_put(WAIT_RES, 0); // wait_res low to reset the flip-flop

    init_databus();
    init_addressbus();

    sleep_ms(100);

    if (debug) test_memory();

    zero_memory();
    load_stage1_bootloader();

    if(debug) dump_memory_to_stdout();

    // set WR, RD and MREQ as inputs with pull-up
    gpio_set_dir(RD, GPIO_IN);
    gpio_pull_up(RD);
    gpio_set_dir(WE, GPIO_IN);
    gpio_pull_up(WE);
    gpio_set_dir(MREQ, GPIO_IN);
    gpio_pull_up(MREQ);

#ifdef PIO_CLOCK_ENABLED
    start_clock();
#else
    gpio_init(CLK);
    gpio_set_dir(CLK, GPIO_OUT);
    slow_clock_init();
#endif
}

void handle_io_write() {
    // read Z80's address bus
    uint32_t io_address = read_from_addressbus();
    uint8_t io_data = read_from_databus() & 0xff;
    if (debug2) printf("DEBUG: WR: IO address: %02lx, IO Data: %02x\r\n", io_address, io_data);

    switch (io_address) {
        case 0x00:
            printf("DEBUG: IO Address 0x00. No operation implemented\r\n");
            break;
        case 0x01:
            if (debug2) printf("Serial TX requested\r\n");
            piper_uart_putc(io_data);
            break;
        case 0x09:
            // disk emulation, SELDISK - select the disk number:
            if (debug)
                printf("DEBUG: Disk selection requested (d=%02x)\r\n", io_data);

            if (io_data < 4)
                piper_set_disk_sel(io_data);
            break;
        case 0x0a:
            // disk emulation, SETTRK - set the track number:
            // word split in 2 bytes.
            if (debug && track_byte_sel == 0)
                printf("DEBUG: Set track number requested (t=%02x)\r\n", io_data);

            if (track_byte_sel == 0) {
                track_sel = io_data;
                track_byte_sel++;
            } else {
                // track_sel is a 16-bit word
                track_sel = (io_data << 8) | (track_sel & 0xff);
                track_byte_sel = 0;
                piper_set_track((track_sel & 0xff));
            }
            break;
        case 0x0b:
            // disk emulation, SETSEC - set the sector number:
            // word split in 2 bytes.
            if (debug && sector_byte_sel == 0)
                printf("DEBUG: Set sector number requested (s=%02x)\r\n", io_data);

            if (sector_byte_sel == 0) {
                sector_sel = io_data;
                sector_byte_sel++;
            } else {
                sector_sel = (io_data << 8) | (sector_sel & 0xff);
                sector_byte_sel = 0;
                piper_set_sector((sector_sel & 0xff));
            }
            break;
        case 0x0c:
            // disk emulation, WRITESEC - write the sector to the disk
            // write 128 subsequent data bytes to the current disk/track/sector
            if (debug && sector_byte_counter == 0)
                printf("DEBUG: Write sector to disk, counter = %d\r\n", sector_byte_counter);

            // collect data ..
            if (sector_byte_counter < SECTOR_SIZE) {
                *(sector_buffer + sector_byte_counter++) = io_data;
            }

            // and send it
            if (sector_byte_counter == SECTOR_SIZE - 1)
                piper_write_floppy_sector(sector_buffer, SECTOR_SIZE);
            break;
        default:
            printf("DEBUG: Write request from unknown or not implemented IO address: %02lx\r\n", io_address);
    }

    // control bus sequence to exit from a wait state
    gpio_put(BUSREQ, 0);         // Request for a DMA
    gpio_put(WAIT_RES, 0);       // Reset WAIT flip-flop exiting from the wait state
    sleep_us(10);                // TODO: 10us might be too much. Keep an eye on this
    gpio_put(WAIT_RES, 1);       // now the Z80 is in DMA, so it's safe to set wait_res to 1
    gpio_put(BUSREQ, 1);         // resume normal operation
}


void handle_io_read() {
    uint32_t io_address = read_from_addressbus();
    uint8_t  io_data = 0x00;

    if (debug2) printf("DEBUG: Read operation from address %02lx requested\r\n", io_address);

    switch (io_address) {
        case 0x00:
            if (debug) printf("DEBUG: IO Address 0x00. No operation implemented\r\n");
            break;
        case 0x01:
            // NOTE 1: if there is no input char, a value 0xFF is forced as input char.
            // NOTE 2: the INT_ signal is always reset (set to HIGH) after this I/O operation.

            if (debug2) printf("Serial RX requested\r\n");

            io_data = 0xff;
            if (uart_char != '\0') {
                io_data = uart_char;
                uart_char = '\0';
            }
            if (Z80_interrupt_flag)
                gpio_put(INT, 1);
            break;
        case 0x02:
            // read boot phase 2 payload
            if (index_stage2 < stage2_size)
                io_data = *(stage2 + index_stage2++);
            break;
        case 0x05:
            // disk emulation, ERRDISK - read the error status of the disk
            if (debug) printf("DEBUG: Read disk error status (%02x)\r\n", disk_error);
            io_data = disk_error;
            break;
        case 0x06:
            // disk emulation, READSEC - read the sector from the disk
            // read 128 subsequent data bytes from the current disk/track/sector
            if (debug && sector_byte_counter == 0)
                printf("DEBUG: Read sector from disk\r\n");

            // read the sector from the floppy, if not already read
            if (sector_byte_counter == 0)
                piper_read_floppy_sector();

            // data is in the sector_buffer, send it to the Z80
            if (sector_byte_counter < SECTOR_SIZE) {
                io_data = *(sector_buffer + sector_byte_counter++);
            } else {
                printf("DEBUG: Sector read complete\r\n");
                disk_error = 0x09; // I/O byte counter overrun
            }
            break;

        case 0x07:
            // sysflags (various system flags for the OS)
            // currently, seeting only autoexecFlag to false:
            io_data = 0x00;
            break;

        default:
            printf("DEBUG: Read request from unknown or not implemented IO address: %02lx\r\n", io_address);
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
}


int main() {

    initialize_pi80();

    // release reset. Z80 should start executing code from address 0x0000
    uart_printf("Stage 1 bootloader loaded. Press any key to release Z80 from the reset.\r\n");
    uart_char = '\0';
    while (uart_char == '\0') {
        sleep_ms(100);
    }
    uart_char = '\0';
    uart_printf("\r\n");

    sleep_ms(200);
    gpio_put(WAIT_RES, 1);
    gpio_put(BUSREQ, 1);
    sleep_ms(200);
    gpio_put(RST, 1);

    printf("DEBUG: Z80 released from reset. Waiting for IO operations.\r\n");

    while (true) {
        // IO operation requested
        if (gpio_get(WAIT) == 0) {
            // Write operation requested
            if (gpio_get(WE) == 0) {
                handle_io_write();
            }
            else if (gpio_get(RD) == 0) {
                handle_io_read();

            } else {
                if (debug2) printf("DEBUG: INT operation\r\n");

                gpio_put(BUSREQ, 0);                       // Request for a DMA
                gpio_put(WAIT_RES, 0);                     // Reset WAIT flip-flop exiting from the wait state
                sleep_us(10);
                gpio_put(WAIT_RES, 1);                     // now the Z80 is in DMA, so it's safe to set wait_res to 1
                gpio_put(BUSREQ, 1);                       // resume normal operation

            }
        }
    }
}

#pragma clang diagnostic pop