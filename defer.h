#ifndef DEFER_H 
#define DEFER_H

#define _POSIX_C_SOURCE 200809L
#define RED "\033[31m"
#define DEFAULT "\033[39m"
#define COMPILE_FILE "command_t.txt"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int ends_with_c(const char * string); 
int ends_with_star_c(const char * string); 

#endif