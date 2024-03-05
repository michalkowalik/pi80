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
bool sector_read_complete = false;

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
}

void piper_set_disk_sel(uint8_t disk) {
    disk_sel = disk;
    uint8_t header[] = {1, 1};
    uart_write_blocking(UART_ID, header, 2);
    uart_write_blocking(UART_ID, &disk, 1);
}

void piper_read_floppy_sector() {
    uint8_t request[] = {0x07, SECTOR_SIZE};
    uart_write_blocking(UART_ID, request, 2);

    // wait for the floppy to respond
    while (!sector_read_complete) {
        sleep_us(10);
    }

    // at this point, the sector buffer should be filled with the data
    sector_read_complete = false;

}

void piper_write_floppy_sector(uint8_t *data, uint8_t data_length) {
    uint8_t request[] = {0x06, data_length};
    uart_write_blocking(UART_ID, request, 2);
    uart_write_blocking(UART_ID, data, data_length);
}

#pragma clang diagnostic pop