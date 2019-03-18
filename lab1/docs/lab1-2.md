# OS (H) 2019 - Lab 1 Part 2 Report

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
export PATH="~/rpitools/bin:$PATH"
```

## 3. Compile the Raspberry Pi Linux kernel for the first time

Following the lab guide, compiling the Linux kernel is straightforward:

```shell
export ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf
make bcm2708-defconfig
```
