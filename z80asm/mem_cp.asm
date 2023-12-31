.memorymap
defaultslot 0
slotsize $8000
slot 0 $0000
.endme

.rombankmap
bankstotal 1
banksize $8000
banks 1
.endro

; Define the destination address
destination_address:
.dw 0x80
destination_addressEnd:

; Define the string length
string_length:
.dw $0f
string_lengthEnd:


LD HL, source_address       ; Load the source address into HL
LD DE, destination_address  ; Load the destination address into DE
LD BC, string_length      ; Load the string length into BC

copy_loop:
   LD A, (HL)             ; Load a byte from the source address into A
   LD (DE), A             ; Store the byte into the destination address
   INC HL                ; Increment the source address
   INC DE                ; Increment the destination address
   DEC BC                ; Decrement the string length counter
   JP NZ, copy_loop       ; Jump to the copy_loop if the counter is not zero
HLT

; Store the string "(HELLO, WORLD!)" in the data section
string_data:
.db $28, $48, $45, $4c, $4c, $4f, $2c, $20, $57, $4f, $52, $4c, $44, $21, $29, $00
string_dataEnd:

; Define the source address
source_address:
.dw string_data
source_addressEnd:
