org 0x00

; print a string to the screen

ld hl, text
call puts

reads:
  in a, (1)
  out (1), a

halt


puts:
  ld a, (hl)
  cp 0x00              ; end of message reached?
  jr z, puts_end
  out (1), a           ; print character
  inc hl
  jr puts
puts_end:
  ret


text:
  dm "Hello World from Z80!", 0x0A, 0x00
