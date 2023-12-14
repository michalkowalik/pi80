//
// Created by Michal Kowalik on 27.11.23.
//
#include "hardware/pio.h"


#ifndef PI80_PIO_CLOCK_H
#define PI80_PIO_CLOCK_H

#endif //PI80_PIO_CLOCK_H

void start_clock();
void init_databus();
void init_addressbus();
void send_to_databus(uint32_t data);
uint32_t get_from_databus();
void send_to_addressbus(uint8_t address);