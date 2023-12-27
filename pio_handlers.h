//
// Created by Michal Kowalik on 27.11.23.
//
#include "hardware/pio.h"


#ifndef PI80_PIO_CLOCK_H
#define PI80_PIO_CLOCK_H

#endif //PI80_PIO_CLOCK_H

#define CLK_FREQ 4000

static const uint DataBusSM = 0;
static const uint AddressBusSM = 1;
static const uint ClockSM = 2;
static const uint WriteIRQ = PIO0_IRQ_0;

extern bool is_written;
extern PIO ClockPio;
extern PIO BusPio;

void start_clock();
void init_databus();
void init_addressbus();
void send_to_databus(uint32_t data);
uint32_t get_from_databus();
void send_to_addressbus(uint8_t address);