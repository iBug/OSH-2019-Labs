// File: globals.h
// Author: iBug

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stddef.h>
#include "config.h"

extern int server_sock;
extern char cwd[MAX_PATH_LEN];
extern char document_root[MAX_PATH_LEN];
extern size_t cwd_len;
extern size_t document_root_len;

#endif
