bin = kernel.img
elf = kernel.elf
obj = kernel.o
code = turn_led_on.s #sysTimer.s

PLATFORM ?= linux
ABI ?= gnueabihf
OBJCOPY := arm-${PLATFORM}-${ABI}-objcopy
LD := arm-${PLATFORM}-${ABI}-ld
AS := arm-${PLATFORM}-${ABI}-as

.PHONY: all

all: $(bin)

$(bin): $(elf)
	${OBJCOPY} $(elf) -O binary $(bin)

$(elf): $(obj)
	${LD} $(obj) -o $(elf) 

$(obj): $(code)
	${AS} -g -o $(obj) $(code)

clean:
	rm -f $(obj) $(elf) $(bin)
