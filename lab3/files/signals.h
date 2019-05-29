// File: signals.h
// Author: iBug

#ifndef _SIGNALS_H
#define _SIGNALS_H

// Master register
void register_signal_handlers(void);

// Individual handlers
void on_SIGINT(int);

#endif
