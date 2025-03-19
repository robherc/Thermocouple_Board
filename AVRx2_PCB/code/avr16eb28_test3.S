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
        STS     PORTA_DIR,      R16             ; Cleanup any previous pin assignments
        STS     PORTC_DIR,      R16
        STS     PORTD_DIR,      R16
        EOR     R1,             R1
        INC     R1                              ; Set R1 as "1 Register"

; Setup timer TCF to output 150khz on PA6
        LDI     R16,            0x40            ; PA6
        LDS     R17,            PORTA_DIR
        OR      R16,            R17
        STS     PORTA_DIR,      R16             ; Set Pin A6 to OUTPUT
        STS     TCFROUTEA,      R1              ; Set TCF WO0 to PA6
        LDI     R16,            0x07
        STS     TCF0_CTRLB,     R16             ; WGM 8-bit PWM
        STS     TCF0_CTRLC,     R1              ; Enable WO0 OUTPUT
        LDI     R16,            16
        STS     TCF0_CNT1,      R16             ; 36 top count (5MHz/17 = 294kHz)
        LDI     R16,            4
        STS     TCF0_CMP0,      R16             ; 23.5% Duty-Cycle
        STS     TCF0_CTRLA,     R1              ; Enable TCF0 (last)

nop_loop:
        nop
        RJMP nop_loop
