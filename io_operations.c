//
// Created by Michal Kowalik on 07.01.24.
// Implements Pi80per IO operations
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <hardware/uart.h>
#include "pins.h"

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