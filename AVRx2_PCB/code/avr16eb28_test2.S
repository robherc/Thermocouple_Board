#include "avr16eb28defs.inc"


        .org 0x00                               ; Stupid to put executable code in the interrupts queue, but let's live dangerously
        .global main
main:
        LDI     R16,            0xD8            ; CCP Enable Write to Protected Registers value
        LDI     R17,            0x03            ; !PBDIV, PDIV/4, PEN
        LDI     R18,            0X05            ; TIMEBASE divisor for 1uS (5 because we're now setting 5MHz clock)
        STS     CPU_CCP,        R16             ; Disable CCP protection for 4 cycles
        STS     MCLKCTRLB,      R17             ; Set Clock Divider to /4 (5MHz)
        STS     MCLKTIMEBASE,   R18             ; Set TIMEBASE constant
        EOR     R16,            R16
        EOR     R17,            R17
        EOR     R18,            R18             ; Zero these registers, while waiting for the clock to settle

; Stupid-simple loop to transparently message-pass BTWN MISO<--UART RX & MOSI-->UART TX
        LDI     R16,            0x01            ; Pin1 = PORTA PIN0
        STS     PORTA_DIR,      R16             ; Set UART TX to OUTPUT

loop:
        LDS     R16,            PORTA_IN
        ANDI    R16,            0x02
        STS     PORTC_OUT,      R16             ; Copy-state from UART RX to MISO
        LDS     R16,            PORTC_IN
        ANDI    R16,            0x01
        STS     PORTA_OUT,      R16             ; Copy-state from MOSI to UART TX
        rjmp    loop                            ; Repeat
