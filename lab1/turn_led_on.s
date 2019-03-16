.section .init
.global _start
@------------------
@SETUP VALUES
@------------------
.equ BASE, 0x3F200000 @ Base
.equ GPFSEL2, 0x08
.equ GPSET0, 0x1C
.equ GPCLR0, 0x28
.equ SET_BIT27, 0x08000000
.equ SET_BIT29, 0x20000000
@------------------
@Start label
@------------------
_start:
@------------------
@load register with BASE
@------------------
ldr r0, =BASE
@------------------
@Set bit 27 in GPFSEL2
@------------------
ldr r1, =SET_BIT27
str r1, [r0, #GPFSEL2]
@------------------
@Set bit 29 in GPSET0
@------------------
ldr r1, =SET_BIT29
str r1, [r0, #GPSET0]
