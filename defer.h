#ifndef DEFER_H 
#define DEFER_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define RED "\033[31m"
#define PURPLE "\033[35m"
#define DEFAULT "\033[39m"
#define COMPILE_FILE "command_t.txt"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair
{
    int first; 
    int second; 
} pair; 

typedef struct revert_info
{
    char * filename; 
    char * content; 
} revert_info; 

void modify_file(const char *); 
void revert_file(const char *); 
void strip_whitespace(char *); 
void find_scopes(const char *, pair *); 
void find_and_replace_defer(const char *, pair *);
int ends_with(const char *, const char *); 
unsigned int get_file_size(FILE *); 

#endif
