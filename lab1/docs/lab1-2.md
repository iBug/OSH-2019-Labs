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

Or if GitHub is too slow for you and you want to reduce download size:

```shell
svn checkout https://github.com/raspberrypi/tools/trunk/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf tools
```

Browse `tools/` and you'll be surprised how many there are. Ignore the rest and take what we need here: `tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/`. Copy this toolset to somewhere convenient, for example `~/rpitools`, and add it to `$PATH` for later use:

```shell
export PATH="~/rpitools/bin:$PATH"
```

## 3. Compile the Raspberry Pi Linux kernel for the first time

Following the lab guide, compiling the Linux kernel is straightforward:

```shell
export ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf
make bcm2709_defconfig
make -j8 zImage
```

The first line exports information about cross-compilation which `make` will respect. This saves some command-line typing. The second line creates the default config for Raspberry Pi (Broadcom BCM2709 equivalent) and the last line builds the kernel with the default config, with a maximum of 8 concurrent jobs.

On my machine, the compilation took 12 minutes, before I could grab the output file `arch/arm/boot/zImage` and place it in the root of the SD card with the name `kernel7.img`. Then power up the Raspberry Pi.

## Answers to questions

1. In addition to `kernel7.img`, the following 4 files are necessary for a minimal Linux system to boot up:

  - `botocode.bin`
  - `start.elf`
  - `config.txt`
  - `cmdline.txt`

  Their functionalities are described in the report of Lab 1-1.

2. An FAT32 filesystem is used. As described in the report of Lab 1-1, it's possible to use an FAT16 volume for this.

3. The volume of the root mountpoint (`/`) is specified as a kernel parameter in `cmdline.txt`:

  ```text
  dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=PARTUUID=7ee80803-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait quiet init=/init
  ```

  The 4th option, `root`, specifies a volume for the root mountpoint, which reads `7ee80803-02`, indicating the 2nd partition on a hard drive with ID `7ee80803`, which resolves to `/dev/sdc2`. The init program is also specified in the kernel parameter, which is the last one in the file: `init=/init`. This means the init program is located at path `/init`, so the kernel will load and execute it, with its PID being 1.

4. What kernel features must be enabled for `init` to execute? This appears rather obvious, but giving a precise answer isn't as easy as it seems. At a bare minimum, kernel support for block layer (MBR partition schema), ext4 filesystem, ELF binaries, LED triggers, GPIO triggers, GPU support and video output via HDMI, tty must be enabled. Some other features required by the kernel, such as at least one floating-point emulation mode, must also be enabled as well.

  By backing up and retrying, I managed to compile a kernel that's only 908 KiB in size.

5. Because the `init` program is run with PID 1 and is supposed to run forever.

  Reading from line 579 of `kernel/exit.c`, the kernel will panic upon `init` exiting.
