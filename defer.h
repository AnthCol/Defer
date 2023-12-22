#ifndef _DEFER_H
#define _DEFER_H
#define _POSIX_C_SOURCE 200809L


#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <utility>
#include <iostream>
#include <pthread.h>
#include <filesystem>

typedef struct flag_container 
{
    bool recursive; 
    bool backwards; 
    bool help; 
    bool version; 
} flag_container; 

typedef struct scope_data
{
    int line_begin; 
    int line_end; 
    bool has_return; 
} scope_data; 


void * forwards(void *); 
void * backwards(void *); 
void get_c_files(std::vector<std::string>&, int, char **); 
void recursive_get_c_files(std::vector<std::string>&); 
void set_flag(flag_container&, std::string); 
void determine_flags(flag_container&, int, char **); 
void print_help(); 
void print_version(); 
bool ends_with(std::string, std::string); 

#endif

