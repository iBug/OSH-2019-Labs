// File: signals.c
// Author: iBug

#include "signals.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "globals.h"
#include "util.h"

void register_signal_handlers(void) {
    signal(SIGINT, on_SIGINT);
}

void on_SIGINT(int signal) {
    eprintf("Shutting down...\n");
    close(server_sock);
    _exit(0);
}
