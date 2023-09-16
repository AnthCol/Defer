#ifndef DEFER_H 
#define DEFER_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define MAX_THREADS 64
#define RED "\033[31m"
#define PURPLE "\033[35m"
#define DEFAULT "\033[39m"
#define SEARCH_COMMAND "find . -type f -name '*.c'"
#define COMPILE_FILE "compile.defer"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct revert_pair
{ 
    char * filename; 
    char * contents;  
} revert_pair; 

int ends_with(const char *, const char *); 
int already_processed(revert_pair *, const char *, int); 
int get_file_size(FILE *); 
int cmp_func(const void *, const void *); 
void strip_whitespace(char *); 
void save_original_file(revert_pair *, const char *, int *); 
void copy_file_data(FILE *, const char *, revert_pair *, int); 
void * modify_file(void *); 
void * revert_file(void *); 

#endif
