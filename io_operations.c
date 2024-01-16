//
// Created by Michal Kowalik on 07.01.24.
// TODO: Implement this
//

#include <stdio.h>
#include <stdarg.h>
#include <hardware/uart.h>
#include "pins.h"


void uart_printf(const char *format, ...) {
    va_list args;
    char buffer[256];
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);

    uart_puts(UART_ID, buffer);
}
