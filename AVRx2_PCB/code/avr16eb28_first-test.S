.org 0x00                     ; Stupid to put executable code in the interrupts queue, but let's live dangerously
LDI   R16,          0xD8      ; CCP Enable Write to Protected Registers value
LDI   R17,          0x03      ; !PBDIV, PDIV/4, PEN
LDI   R18,          0X05      ; TIMEBASE divisor for 1uS (5 because we're now setting 5MHz clock)
STS   CPU.CCP,      R16       ; Disable CCP protection for 4 cycles
STS   MCLKCTRLB,    R17       ; Set Clock Divider to /4 (5MHz)
STS   MCLKTIMEBASE, R18       ; Set TIMEBASE constant
XOR   R16,          R16
XOR   R17,          R17
XOR   R18,          R18       ; Zero these registers, while waiting for the clock to settle

; Stupid-simple loop to toggle pin 1 @ 500kHz as a first-test
LDI   R16,          0x80      ; Pin1 = PORTA PIN7
STS   PORTA.DIR,    R16       ; Set pin1 to OUTPUT

:loop
nop
nop
STS   PORTA.OUTTGL, R16       ; Set the bit to toggle the pin
rjmp loop                     ; Repeat
