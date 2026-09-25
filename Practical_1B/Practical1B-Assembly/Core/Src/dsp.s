/*
 * dsp.s
 * EEE3096S 2026 - Practical 1B, Task 4
 * Cycle-counted ADC to DAC loop with a 45 degree phase delay
 *
 * Student 1 : Raman Raja  RJXRAM001
 * Student 2 : Rojan Jameson  JMSROJ001
 */

    .syntax unified
    .thumb
    .cpu    cortex-m0
    .fpu    softvfp

    .global DSP_Loop
    .type   DSP_Loop, %function

@ ---------------------------------------------------------------------------
@ Peripheral addresses
@ ---------------------------------------------------------------------------
    .equ ADC_DR,      0x40012440
    .equ DAC_DHR12R1, 0x40007408

    .section .text.DSP_Loop, "ax", %progbits

@ ===========================================================================
@ ENTRY POINT
@ ===========================================================================
DSP_Loop:
    @ Setup registers outside the timed loop
    LDR R0, =ADC_DR
    LDR R1, =DAC_DHR12R1

loop:
    @ --- SAMPLE AND OUTPUT ------------------------------------------------
    @ TODO 1: Read the current ADC conversion from the Data Register.
    LDR R2, [R0]       @ 2 cycles

    @ --- DELAY SETUP ------------------------------------------------------
    @ TODO 3: Calculate the required cycle target for a 45-degree phase 
    @         delay on a 1 kHz sine wave running at an 8 MHz system clock.
    @         Load your inner loop counter and insert any NOP padding 
    @         needed to hit your exact target.




    @ The mathematics behind my implementation:
    @ Target = 125us = 125000 ns. Cycle = 125 ns. Target = 1000 cycles.
    @ Loop overhead = 2(LDR) + 1(MOVS) + 2(STR) + 3(B loop) = 8 cycles
    @ Target for padding + delay_loop = 1000 - 8 = 992 cycles
    @ A 4-cycle loop (SUBS+BNE) running 248 times takes:
    @ (247 * 4) + (1 * 2) = 988 + 2 = 990 cycles.
    @ We need 2 more cycles of padding, so we add two NOPs.
    MOVS R3, #248    @ 1 cycle (inner loop counter)
    NOP                @ 1 cycle
    NOP                @ 1 cycle

delay_loop:
    @ --- INNER LOOP -------------------------------------------------------
    @ TODO 4: Implement the counted delay loop.
    @         (Remember to use flag-updating arithmetic so your branch works).
    SUBS R3, R3, #1    @ 1 cycle
    BNE delay_loop     @ 3 cycles (taken), 1 cycle (not taken on last loop)

    @ --- OUTPUT -----------------------------------------------------------
    @ TODO 2: Write the value straight out to the DAC Data Register.
    @ (I moved it here AFTER the delay to actually create the phase shift, not sure why it was right after step 1)
    STR R2, [R1]       @ 2 cycles

    @ --- REPEAT -----------------------------------------------------------
    @ TODO 5: Branch back to the start of the main 'loop'.
    B loop             @ 3 cycles
    
    @ ----------------------------------------------------------------------
    @ NOTE: You must calculate your exact cycle budget, showing the cost 
    @ of every instruction and loop iteration, and document it in your report.
    @ ----------------------------------------------------------------------

    .size DSP_Loop, .-DSP_Loop