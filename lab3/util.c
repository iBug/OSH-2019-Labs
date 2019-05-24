// File: util.c
// Author: iBug

#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void errorexit(const char* s) {
    if (errno == 0) {
        // Huh?
        perror(s);
    } else {
        perror(s);
        _exit(1);
    }
}
