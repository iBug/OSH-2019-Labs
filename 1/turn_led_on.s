.section .init
.global _start

.equ GPIO_BASE, 0x3F200000
.equ GPFSEL2, 0x08
.equ GPSET0, 0x1C
.equ GPCLR0, 0x28
.equ SET_BIT27, 0x08000000
.equ SET_BIT29, 0x20000000

_start:
ldr r0, =GPIO_BASE

mov r1, #SET_BIT27
str r1, [r0, #GPFSEL2]

mov r1, #SET_BIT29
str r1, [r0, #GPSET0]
