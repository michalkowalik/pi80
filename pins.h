//
// Created by Michal Kowalik on 28.11.23.
// Hardware definitions for the Pi Pico
//

#ifndef PI80_PINS_H
#define PI80_PINS_H

#endif //PI80_PINS_H

#define UART_ID uart0
#define BAUD_RATE 921600
//#define BAUD_RATE 115200

// Data bus
#define D0 0
#define DATA_BUS_WIDTH 8

// Address bus
#define A0 8
#define ADDRESS_BUS_WIDTH 6

// control pins
#define BUSREQ 14
#define BUSACK 15
#define UART_TX 16
#define UART_RX 17
#define CLK 18
#define INT 19
#define RST 20
#define WAIT 21
#define RD 22
#define LED 25
#define WE 26
#define MREQ 27
#define WAIT_RES 28


