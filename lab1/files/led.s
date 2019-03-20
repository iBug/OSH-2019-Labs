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
.equ SECOND, 1000000

_start:
ldr r0, =BASE

@ Enable GPIO 29 for output (On-board green LED)
ldr r1, =BIT27
str r1, [r0, #GPFSEL2]

ldr r1, =BIT29

@ Load System Timer
ldr r2, =TIMER_BASE
ldr r4, =0
ldr r5, [r2, #GPCLO]
ldr r6, =SECOND

loopstart:

add r5, r5, r6
wait1: @ Poll the clock until enough time has passed
ldr r3, [r2, #GPCLO]
cmp r3, r5
ble wait1

@ Enable the LED
str r1, [r0, #GPSET0]
str r4, [r0, #GPSET0]

add r5, r5, r6
wait2: @ Poll the clock again
ldr r3, [r2, #GPCLO]
cmp r3, r5
ble wait2

@ Clear the LED
str r1, [r0, #GPCLR0]
str r4, [r0, #GPCLR0]
b loopstart
