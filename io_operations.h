//
// Created by Michal Kowalik on 16.01.24.
//

#ifndef PI80_IO_OPERATIONS_H
#define PI80_IO_OPERATIONS_H

void uart_printf(const char *format, ... );

void piper_uart_puts(const char *str);
void piper_uart_putc(uint8_t c);

#endif //PI80_IO_OPERATIONS_H
