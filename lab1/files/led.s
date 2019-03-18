.section .init
.global _start

.equ BASE, 0x3F200000 @ GPIO Base
.equ GPFSEL2, 0x08
.equ GPSET0, 0x1C
.equ GPCLR0, 0x28

.equ TIMER_BASE, 0x3F003000 @ System Timer Base
.equ GPCLO, 0x04

.equ BIT27, 0x08000000
.equ BIT29, 0x20000000

_start:
ldr r0, =BASE

@ Enable GPIO 29 for output (On-board green LED)
ldr r1, =BIT27
str r1, [r0, #GPFSEL2]

ldr r1, =BIT29

@ Load System Timer
ldr r2, =TIMER_BASE
ldr r4, =0

loopstart:
ldr r3, [r2, #GPCLO]

and r3, r3, #0x00080000
cmp r3, #0
bne led_off

led_on:
str r1, [r0, #GPSET0]
str r4, [r0, #GPSET0]
b loopstart

led_off:
str r1, [r0, #GPCLR0]
str r4, [r0, #GPCLR0]
b loopstart
