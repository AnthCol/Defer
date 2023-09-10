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

typedef struct defer_location
{
    char * instruction;
    int line; 
} defer_location; 

typedef struct location_info
{
    pair * scope; 
    defer_location * defer; 
    int defer_size; 
    int scope_size; 
} location_info; 

typedef struct revert_info
{
    char * filename; 
    char * content; 
} revert_info; 

void single_file(const char *); 
void multiple_files(const char *);
void revert_single(const char *); 
void revert_multiple(const char *); 
void find_scope_and_defer(const char *, location_info *); 
void substitute_defer(const char *, location_info *); 
void strip_whitespace(char *); 
int ends_with(const char *, const char *); 
int check_defer_syntax(const char *); 
unsigned int get_file_size(FILE *); 

#endif
