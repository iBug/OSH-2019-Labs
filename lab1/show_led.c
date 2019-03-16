// File: show_led.c
// Author: iBug

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>

int main() {
    uint32_t offset = 0x3F200000;
    uint32_t FSEL2 = 0x08,
             SET0 = 0x1C,
             CLR0 = 0x28;

    int fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd_mem < 0) {
        puts("Cannot open /dev/mem");
        return 1;
    }
    void *m = mmap(0, 0x80, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, offset);
    *(uint32_t*)(m + FSEL2) = (1 << 27);
    for (int i = 0; i < 100000000; i++) {
        *(uint32_t*)(m + SET0) = (1 << 29);
    }
    for (int i = 0; i < 100000000; i++) {
        *(uint32_t*)(m + CLR0) = (1 << 29);
    }
    for (int i = 0; i < 100000000; i++) {
        *(uint32_t*)(m + SET0) = (1 << 29);
    }
    munmap(m, 0x80);
    close(fd_mem);
    return 0;
}
