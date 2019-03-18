# OS (H) 2019 - Lab 1 Part 1 Report

付佳伟 PB17111643

## 1. The boot process of Raspberry Pi

According to [How does Raspberry Pi boot?][1], the booting process of Raspberry Pi Foundation's official OS, Raspbian, is as follows:

1. When the Pi is first powered up, the ARM cores are off, and the VideoCore IV GPU core loads the ROM embedded in the chip. The ROM is the first-stage bootloader.
2. The GPU ROM reads the microSD card and locates the boot partition, which is a FAT32 or FAT16 volume<sup>\[2\]</sup>, then finds the second-stage bootloader file `bootcode.bin` and loads it into the L2 cache of the SoC, and executes it.
3. `bootcode.bin` loads the third-stage bootloader `start.elf`.
4. `start.elf` parses `config.txt` and initialize core hardware, including enabling the ARM CPU cores and allocating memory between the CPU and the GPU, before parsing `cmdline.txt` and loading the Linux kernel `kernel.img`, which then loads everything else, such as `init` and `systemd`.

## 2. The similarity and difference between the boot process of Raspberry Pi and a Intel x86-based PC

First, because the Raspberry Pi uses a System-on-Chip (SoC), it is more integrated than a x86-based PC. For example its CPU, GPU and first-stage bootloader is grained on one silicon chip. On the contrary, the first-stage bootloader equivalent of a x86 PC, the basic input/output system (BIOS), is stored on another chip than the CPU, usually an EEPROM, which makes it modifiable (you surely have seen the term "BIOS upgrade", which is exactly when the BIOS is modified).

From here, the boot process of a typical PC starts to divert, where there are two major streamlines of methods, BIOS and UEFI. Despite, they're fundamentally identical, as both loads the initial bit of code from a piece of special designated hardware (a ROM), which initializes hardware before loading further boot code from hard drive.

If I must say, I'd say that the boot process of the Raspberry Pi is more akin to that of UEFI, as both searches for specific partitions on the selected hard drive. BIOS, on the other hand, loads code from a fixed point on the hard drive (the first 512 bytes, called the master boot record (MBR)).

## 3. What filesystems are accessed during the boot process of the Raspberry Pi

According to BeyondLogic<sup>\[2\]</sup>, the boot partition has a filesystem of FAT32 or FAT16. Then depending on the kernel and the operating system, addition filesystems are accessed, for example ext4 (Raspbian) or NTFS (Windows 10 on ARM).

## References:

1. syb0rg (November 2, 2013), [How does Raspberry Pi boot?][1], *Raspberry Pi Stack Exchange*
2. [Understanding the Raspberry Pi Boot Process][2], *BeyondLogic*
3. Rod Smith (November 6, 2012), [What is the difference in "Boot with BIOS" and "Boot with UEFI"][3], *Super User*
4. The Raspberry Pi Foundation, [How to boot from a USB mass storage device on a Raspberry Pi][4]

<!-- Links -->

  [1]: https://raspberrypi.stackexchange.com/a/10490/68608
  [2]: https://wiki.beyondlogic.org/index.php?title=Understanding_RaspberryPi_Boot_Process
  [3]: https://superuser.com/a/501867/688600
  [4]: https://www.raspberrypi.org/documentation/hardware/raspberrypi/bootmodes/msd.md
