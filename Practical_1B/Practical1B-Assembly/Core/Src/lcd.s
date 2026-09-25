/*
 * lcd.s
 * EEE3096S 2026 - Practical 1B, Task 5
 * 4-bit bit-banged HD44780 driver, and the level shifter timing fault
 *
 * Student 1 : Raman Raja  RJXRAM001
 * Student 2 : Rojan Jameson  JMSROJ001
 */

    .syntax unified
    .thumb
    .cpu    cortex-m0
    .fpu    softvfp

    .global LCD_Run
    .type   LCD_Run, %function

@ ---------------------------------------------------------------------------
@ Register addresses. BSRR is at offset 0x18 from each port base.
@ ---------------------------------------------------------------------------
    .equ GPIOA_BSRR, 0x48000018
    .equ GPIOB_BSRR, 0x48000418
    .equ GPIOC_BSRR, 0x48000818

@ ---------------------------------------------------------------------------
@ PIN MAP
@   PC15  Enable (E)     -> PC15_S on the 5 V side
@   PC14  Register Select (RS)
@   PB8   D4      PB9   D5      PA12  D6      PA15  D7
@   R/W is tied to ground. The LCD is write only. 
@ ---------------------------------------------------------------------------

    .section .text.LCD_Run, "ax", %progbits

@ ===========================================================================
@ ENTRY POINT
@ ===========================================================================
LCD_Run:
    PUSH {LR}

    @ TODO 1: Wait for the LCD power rail to settle (consult datasheet).
    @ HD44780 requires at least 40ms after VCC rises to 2.7V.
    MOVS R0, #50
    BL   LCD_DelayLong
    
    @ TODO 2: Call the 4-bit initialization sequence.
    BL   LCD_Init
    
    @ TODO 3: Write the character 'A' (0x41) to the display.
    MOVS R0, #0x41
    BL   LCD_WriteData

hang:
    B    hang

    .size LCD_Run, .-LCD_Run

@ ===========================================================================
@ LCD_Init
@ Puts the controller into 4-bit mode and readies the display.
@ ===========================================================================
    .type LCD_Init, %function
LCD_Init:
    PUSH {LR}

    @ TODO 4: Send the 4-bit initialization sequence.
    @ Reference the HD44780 datasheet flowchart. 
    @ Send commands with RS low using LCD_WriteCmd.
    @ Step 1: Send 0x03, wait > 4.1ms
    MOVS R0, #0x03
    BL   LCD_SendNibble
    BL   LCD_Pulse
    MOVS R0, #5
    BL   LCD_DelayLong

    @ Step 2: Send 0x03, wait > 100us
    MOVS R0, #0x03
    BL   LCD_SendNibble
    BL   LCD_Pulse
    MOVS R0, #150
    BL   LCD_DelayShort

    @ Step 3: Send 0x03, wait > 37us
    MOVS R0, #0x03
    BL   LCD_SendNibble
    BL   LCD_Pulse
    MOVS R0, #50
    BL   LCD_DelayShort

    @ Step 4: Send 0x02 to switch to 4-bit mode, wait > 37us
    MOVS R0, #0x02
    BL   LCD_SendNibble
    BL   LCD_Pulse
    MOVS R0, #50
    BL   LCD_DelayShort

    @ Function set: 4-bit, 2 lines, 5x8 font (0x28)
    MOVS R0, #0x28
    BL   LCD_WriteCmd

    @ Display control: Display off (0x08)
    MOVS R0, #0x08
    BL   LCD_WriteCmd

    @ Display clear: (0x01), wait > 1.52ms
    MOVS R0, #0x01
    BL   LCD_WriteCmd
    MOVS R0, #3
    BL   LCD_DelayLong

    @ Entry mode set: Increment, no shift (0x06)
    MOVS R0, #0x06
    BL   LCD_WriteCmd

    @ Display control: Display ON, Cursor OFF (0x0C)
    MOVS R0, #0x0C
    BL   LCD_WriteCmd

    POP {PC}

@ ===========================================================================
@ LCD_WriteCmd   R0 = command byte, RS low
@ LCD_WriteData  R0 = data byte,    RS high
@ Both send the high nibble first, then the low nibble.
@ ===========================================================================
    .type LCD_WriteCmd, %function
LCD_WriteCmd:
    PUSH {R0, LR}
    @ TODO 5: Drive RS (PC14) LOW, then fall through to the shared sender.
    LDR  R1, =GPIOC_BSRR
    LDR  R2, =(1 << 30)       @ Reset PC14 (RS = 0)
    STR  R2, [R1]
    B    LCD_Send8

    .type LCD_WriteData, %function
LCD_WriteData:
    PUSH {R0, LR}
    @ TODO 6: Drive RS (PC14) HIGH, then fall through.
    LDR  R1, =GPIOC_BSRR
    LDR  R2, =(1 << 14)       @ Set PC14 (RS = 1)
    STR  R2, [R1]

LCD_Send8:
    @ TODO 7: Send the upper nibble of R0, pulse Enable,
    @         then the lower nibble of R0, pulse Enable again.
    @ Upper nibble
    PUSH {R0}
    LSRS R0, R0, #4
    BL   LCD_SendNibble
    BL   LCD_Pulse
    POP  {R0}

    @ Lower nibble
    PUSH {R0}
    LDR  R1, =0x0F
    ANDS R0, R0, R1
    BL   LCD_SendNibble
    BL   LCD_Pulse
    POP  {R0}

    @ Generic delay for most commands (>37us)
    PUSH {R0}
    MOVS R0, #50
    BL   LCD_DelayShort
    POP  {R0}

    POP {R0, PC}

@ ===========================================================================
@ LCD_SendNibble   R0 bits 3:0 -> the four data lines
@ ===========================================================================
    .type LCD_SendNibble, %function
LCD_SendNibble:
    PUSH {R1, R2, R3, LR}

    @ TODO 8: Map the four bits of R0 onto the four data pins (across 3 ports).
    @   R0 bit 0 -> PB8   (D4)
    @   R0 bit 1 -> PB9   (D5)
    @   R0 bit 2 -> PA12  (D6)
    @   R0 bit 3 -> PA15  (D7)
    @ Clear all 4 data pins first
    LDR  R1, =GPIOA_BSRR
    LDR  R2, =0x90000000   @ Clear PA15 and PA12
    STR  R2, [R1]

    LDR  R1, =GPIOB_BSRR
    LDR  R2, =0x03000000   @ Clear PB9 and PB8
    STR  R2, [R1]

    @ Load bases for SET operations
    LDR  R1, =GPIOA_BSRR
    LDR  R2, =GPIOB_BSRR

    @ Check Bit 0 (PB8)
    MOVS R3, #1
    TST  R0, R3
    BEQ  skip_bit0
    LDR  R3, =(1 << 8)
    STR  R3, [R2]
skip_bit0:

    @ Check Bit 1 (PB9)
    MOVS R3, #2
    TST  R0, R3
    BEQ  skip_bit1
    LDR  R3, =(1 << 9)
    STR  R3, [R2]
skip_bit1:

    @ Check Bit 2 (PA12)
    MOVS R3, #4
    TST  R0, R3
    BEQ  skip_bit2
    LDR  R3, =(1 << 12)
    STR  R3, [R1]
skip_bit2:

    @ Check Bit 3 (PA15)
    MOVS R3, #8
    TST  R0, R3
    BEQ  skip_bit3
    LDR  R3, =(1 << 15)
    STR  R3, [R1]
skip_bit3:

    POP {R1, R2, R3, PC}

@ ===========================================================================
@ LCD_Pulse
@ ===========================================================================
    .type LCD_Pulse, %function
LCD_Pulse:
    PUSH {R0, R1, R2, LR}

    LDR  R0, =GPIOC_BSRR

    @ TODO 9: Set PC15 HIGH.
    LDR  R1, =(1 << 15)
    STR  R1, [R0]

    @ -----------------------------------------------------------------
    @ TODO 10: THE TIMING FIX
    @ Implement a calculated pad delay here to overcome the RC time 
    @ constant of the level shifter and meet the HD44780 hold time requirements.
    @ Show your cycle arithmetic in the comments.
    @ 450 ns min hold requirement + 108 ns rise time = 558 ns total minimum pulse
    @ 558 ns / 125 ns per cycle = 4.464 cycles needed
    @ We need 5 cycles total. The STR below takes 2 cycles.
    @ Therefore, 3 NOPs provide 3 cycles (375 ns) of delay. 
    @ Total pulse width = 3 (NOPs) + 2 (STR) = 5 cycles = 625 ns.
    @ 625 ns > 558 ns required.
    @ -----------------------------------------------------------------
    NOP
    NOP
    NOP

    @ TODO 11: Set PC15 LOW.
    LDR  R1, =(1 << 31)
    STR  R1, [R0]

    @ TODO 12: Hold Enable low long enough to meet the LCD cycle time.
    PUSH {R0}
    MOVS R0, #2
    BL   LCD_DelayShort
    POP  {R0}

    POP {R0, R1, R2, PC}

@ ===========================================================================
@ Delay helpers
@ ===========================================================================
    .type LCD_DelayLong, %function
LCD_DelayLong:
    @ TODO 13: Implement a millisecond-scale delay. Show cycle arithmetic.
    @ Arithmetic: Inner loop takes 4 cycles (SUBS + BNE).
    @ 4 cycles * 2000 iterations = 8000 cycles.
    @ At 125 ns/cycle (8 MHz), 8000 cycles = 1,000,000 ns = 1 millisecond.
    @ R0 controls the number of milliseconds.
    PUSH {R1, LR}
delay_long_outer:
    CMP  R0, #0
    BEQ  delay_long_done
    LDR  R1, =2000
delay_long_inner:
    SUBS R1, R1, #1
    BNE  delay_long_inner
    SUBS R0, R0, #1
    B    delay_long_outer
delay_long_done:
    POP  {R1, PC}

    .type LCD_DelayShort, %function
LCD_DelayShort:
    @ TODO 14: Implement a microsecond-scale delay. Show cycle arithmetic.
    @ Arithmetic: Inner loop takes 4 cycles (SUBS + BNE).
    @ Outer loop overhead is ~7 cycles.
    @ With R1=2, total per iteration is ~15 cycles.
    @ At 125 ns/cycle, 15 cycles = ~1.875 microseconds per R0 count.
    PUSH {R1, LR}
delay_short_outer:
    CMP  R0, #0
    BEQ  delay_short_done
    MOVS R1, #2
delay_short_inner:
    SUBS R1, R1, #1
    BNE  delay_short_inner
    SUBS R0, R0, #1
    B    delay_short_outer
delay_short_done:
    POP  {R1, PC}