#ifndef DEFER_H 
#define DEFER_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define RED "\033[31m"
#define PURPLE "\033[35m"
#define DEFAULT "\033[39m"
#define COMPILE_FILE "command_t.txt"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ends_with(const char *, const char *); 

#endif
