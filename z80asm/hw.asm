org 0x00

LD hl, text  ; source address
LD de, 0x80  ; destination address
LD bc, 0xf   ; number of bytes to copy

COPY_LOOP:
    LD A, (HL)
    LD (DE), A
    INC HL
    INC DE
    DEC BC
    CP 0x00
    JP NZ, COPY_LOOP
HALT


text:
  dm "Hello, World!", 0x0A, 0x00