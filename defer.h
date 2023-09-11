#ifndef DEFER_H 
#define DEFER_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define RED "\033[31m"
#define PURPLE "\033[35m"
#define DEFAULT "\033[39m"
#define COMPILE_FILE "command.defer"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair
{
    int first; 
    int second; 
} pair; 

typedef struct scope_struct 
{
    pair * pairs; 
    int count; 
} scope_struct; 

typedef struct revert_info
{
    char * filename; 
    char * content; 
} revert_info; 


void modify_file(const char *); 
void revert_file(const char *, revert_info *, int); 
void strip_whitespace(char *); 
void find_scopes(const char *, scope *); 
void find_and_replace_defer(const char *, pair *);

int ends_with(const char *, const char *); 
int end_of_scope(int, scope_struct *); 
unsigned int get_file_size(FILE *); 

#endif
