//
// Created by Michal Kowalik on 16.01.24.
//

#ifndef PI80_IO_OPERATIONS_H
#define PI80_IO_OPERATIONS_H

#define SECTOR_SIZE 0x80

extern uint8_t disk_sel;
extern uint track_byte_counter;
extern uint sector_byte_counter;

extern uint track_sel;
extern uint sector_sel;
extern uint8_t sector_buffer[0x80];
extern bool floppy_operation_complete;

void uart_printf(const char *format, ... );

void piper_uart_puts(const char *str);
void piper_uart_putc(uint8_t c);
void piper_set_disk_sel(uint8_t disk);
void piper_set_sector(uint8_t sector);
void piper_set_track(uint8_t track);
void piper_read_floppy_sector();
void piper_write_floppy_sector(uint8_t *data, uint8_t data_length);

#endif //PI80_IO_OPERATIONS_H
