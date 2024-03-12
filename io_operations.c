//
// Created by Michal Kowalik on 07.01.24.
// Implements Pi80per IO operations
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <hardware/uart.h>
#include <pico/time.h>
#include "pins.h"
#include "io_operations.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "LoopDoesntUseConditionVariableInspection"
uint8_t disk_sel = 0;
uint track_byte_counter = 0;
uint sector_byte_counter = 0;
uint track_sel = 0;
uint sector_sel = 0;
bool floppy_operation_complete = true;

uint8_t sector_buffer[SECTOR_SIZE];   // 128 bytes for the sector buffer

void piper_uart_puts(const char *str) {
    uint8_t header[] = {0, strlen(str)};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, str, strlen(str));
}

void uart_printf(const char *format, ...) {
    va_list args;
    char buffer[256];
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);

    piper_uart_puts(buffer);
}

void piper_uart_putc(uint8_t c) {
    uint8_t header[] = {0, 1};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, &c, 1);

    // wait for the confirmation would be ideal. but we can also wait a quarter of a ms for the uart to send the byte
    sleep_us(250);
}

void piper_set_disk_sel(uint8_t disk) {
    floppy_operation_complete = false;
    disk_sel = disk;
    uint8_t header[] = {2, 1};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, &disk, 1);

    // wait for the floppy to respond
    while (!floppy_operation_complete) {
        sleep_us(10);
    }
}

void piper_set_sector(uint8_t sector) {
    floppy_operation_complete = false;
    sector_sel = sector;
    uint8_t header[] = {3, 1};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, &sector, 1);

    // wait for the floppy to respond
    while (!floppy_operation_complete) {
        sleep_us(10);
    }
}

void piper_set_track(uint8_t track) {
    floppy_operation_complete = false;
    track_sel = track;
    uint8_t header[] = {4, 1};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, &track, 1);

    // wait for the floppy to respond
    while (!floppy_operation_complete) {
        sleep_us(10);
    }

}

void piper_read_floppy_sector() {
    floppy_operation_complete = false;

    // send request to the Pi80per
    uint8_t request[] = {0x07, SECTOR_SIZE};
    uart_write_blocking(UART_ID, request, 2);

    // wait for the floppy to respond
    while (!floppy_operation_complete) {
        sleep_us(10);
    }

    // at this point, the sector_buffer should be filled with the data
}

void piper_write_floppy_sector(uint8_t *data, uint8_t data_length) {
    uint8_t request[] = {0x06, data_length};
    uart_write_blocking(UART_ID, request, 2);
    uart_write_blocking(UART_ID, data, data_length);
}

#pragma clang diagnostic pop