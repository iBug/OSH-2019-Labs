# OS (H) 2019 - Lab 1 Part 3 Report

## 0. Prerequisites

- A Raspberry Pi 3 B+ and its power supply (just get a micro-USB cable and use your computer to power it - it only takes ~ 2 Watt of power)
  - I've heard that this is going to be particularly difficult with 3B due to the target LED we're going to program being hidden behind a virtual GPIO that's only accessible by using a mailbox
- A microSD card of at least 2 GB
- A Linux distro with amd64 architecture. I use Ubuntu on Windows Subsystem for Linux - just for cross-compilation so WSL is up to this job
- Cross-compilation toolchain - see Section 2

## 1. Preparing hardware and dependency files

To allow the Raspberry Pi to boot from the microSD card, you'll need some boot files. They can be fetched from the official Raspbian image (download [here](https://www.raspberrypi.org/downloads/raspbian/), or just skip this if you already have a Raspbian on another microSD card). Extract the `.img` file from the downloaded ZIP and flash it onto the microSD card with

```shell
sudo dd if=/path/to/file.img of=/dev/sdX bs=1M
```

Replace `/path/to/file.img` with the path to the extracted `.img` file, and `sdX` with the device that represents your microSD card.

Then, run `sudo partprobe` to let the kernel refresh partition information, and mount `/dev/sdX1` to somewhere (the first partition is the boot partition). Take these three files from your mounted directory for later use: `bootcode.bin`, `fixup.dat` and `start.elf`.

Because we want to start it clean, we want a clean microSD card. So now format the microSD card with filesystem FAT32 and [a cluster size of 16 KiB or 32 KiB](https://electronics.stackexchange.com/a/407162/176201), and copy the above three files back to the microSD card.

## 2. Setup compilation environment

Setting up the toolchain for cross-compiling for Raspberry Pi 3 B+ isn't quite hard, since the Raspberry Pi Foundation has published a compiled set of toolchain on GitHub. It can be easily fetched with

```shell
git clone https://github.com/raspberrypi/tools.git tools --depth=1 --branch=master
```

Browse `tools/` and you'll be surprised how many there are. Ignore the rest and take what we need here: `tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/`. Copy this toolset to somewhere convenient, for example `~/rpitools`, and add it to `$PATH` for later use:

```shell
export PATH="~/rpitools:$PATH"
```

## 3. Write the first assembly program

Before starting to program, we must know what we're going to program. In this first part, we want to make the green on-board LED light up. Just FYI, the red one is connected to a constant +3.3V power source, and is in fact the power LED and is not programmable. To start off, we would want to find out how to interact with it.

A quick Google Search should be enough to reveal that the green LED is on GPIO 29 of the chip - BCM2837 (Raspberry Pi 3 B+). Then the question turns into communicating via GPIO. It shouldn't be hard to find the [official documentation](https://github.com/raspberrypi/documentation/files/1888662/) for the chip. Go to page 91 and read on - that's the full documentation for the GPIO pins.

BUT, the mapped memory address isn't correct (LOL). According to [this page](https://github.com/bztsrc/raspi3-tutorial), the actual starting address of the GPIO controller on RPi 3 B+ is `0x3F200000`. Substitute `0x7E200000` with this value from now on.

From the same PDF (linked above), we know how we can light up the green LED:

- Set GPIO 29 to "output" mode
- Enable GPIO 29

And here's the code for this task:

```assembly
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
```

Note that due to the limitation on the constant value of `mov` instruction, we have to use `ldr` to load the base address of the GPIO controller. The other constants, however, go along with `mov`.

Save the assembly code as `turn_led_on.s`, and compile it into an OS kernel:

```shell
arm-linux-gnueabihf-as -g -o kernel.o blink_led.s
arm-linux-gnueabihf-ld kernel.o -o kernel.elf
arm-linux-gnueabihf-objcopy kernel.elf -O binary kernel.img
```

The last step is to boot the kernel. Put the generated `kernel.img` onto the microSD card prepared in Section 1, along with `bootcode.bin`, `fixup.dat` and `start.elf`, and insert the microSD card into the Raspberry Pi 3 B+ and power it up. You'll see the green LED flash for once, and light up constantly. That's it.

## 4. Write the second assembly program

Now we want the LED to blink. The first thing you should think of is whether there's some kind of a timer on the Raspberry Pi, and indeed there is.

Read the BCM2837 documentation and find out the details of the system timer. Its starting address should be documented as `0x7E003000`. Again this is wrong, and the same page that has given the correct address for the GPIO controller says the system timer is at `0x3F003000`. Again, use this value.

The system timer runs at 1 MHz, which means just by taking bit 19, you get a single bit that swaps every 524.288 ms, providing a cycle of roughly 1 second. We'll use this as the timer for the LED.

By using a conditional branch to determine whether to write to `GPSETn` or `GPCLRn`, blinking effect can be easily achieved. Here's the full code:

```assembly
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
mov r1, #BIT27
str r1, [r0, #GPFSEL2]

mov r1, #BIT29

@ Load System Timer
ldr r2, =TIMER_BASE
mov r4, #0

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
```

Compile this assembly code into `kernel.img` in the same way above, and copy the kernel to the microSD card to see the effect. The green LED on the Raspberry Pi 3 B+ should be blinking at a frequency of roughly 1 Hz.
