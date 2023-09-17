#include "defer.h"

 
int main(int argc, char ** argv)
{
    int size; 
    int file_count = 0;
    char buf [512]; 
    FILE * fptr; 
    char * temp = malloc(1);
    revert_pair * original_data = malloc(1); 


    temp = realloc(temp, strlen(argv[1]) + 1); 
    strcpy(temp, argv[1]); 

    for (int i = 0; i < (int)strlen(temp); i++)
    {
        temp[i] = toupper(temp[i]); 
    }

    if (strcmp(temp, "ALL") == 0)
    {
        FILE * search_process = popen(SEARCH_COMMAND, "r"); 
        while (fgets(buf, sizeof(buf), search_process) != NULL)
        {
            save_original_file(original_data, buf, &file_count); 
        }
        pclose(search_process); 
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (ends_with(argv[i], ".c"))
            {
                save_original_file(original_data, argv[i], &file_count);  
            } 
            else
            {
                printf("%sFile does not end with .c -> Skipping it%s (%s)\n", PURPLE, DEFAULT, argv[i]); 
            }
        }
    }

    if (file_count > MAX_THREADS)
    {
        printf("%sFile count is greater than MAX_THREADS (%d). Exiting...%s\n", RED, MAX_THREADS, DEFAULT); 
        return 0; 
    }

    pthread_t * thread_handles; 
    thread_handles = malloc(file_count * sizeof(pthread_t)); 

    for (int i = 0; i < file_count; i++)
    {
        pthread_create(&thread_handles[i], NULL, modify_file, (void *) &original_data[i]); 
    }

    for (int i = 0; i < file_count; i++)
    {
        pthread_join(thread_handles[i], NULL); 
    }

    fptr = fopen(COMPILE_FILE, "r"); 
    if (fptr == NULL)
    {
        printf("%sUnable to find \"compile.defer\" file for compiling. Reverting files and exiting.\n%s", PURPLE, DEFAULT); 
    } 
    else
    {
        size = get_file_size(fptr); 
        temp = realloc(temp, size * 2); 
        fread(temp, sizeof(char), size, fptr); 
        fclose(fptr); 

        // FIXME verify compiling file for sudo and rm

        FILE * compile_process = popen(temp, "r"); 
        while (fgets(buf, sizeof(buf), compile_process) != NULL)
        {
            printf("%s", buf); 
        }
        pclose(compile_process); 
    }
    free(temp); 


    for (int i = 0; i < file_count; i++)
    {
        pthread_create(&thread_handles[i], NULL, revert_file, (void *) &original_data[i]);  
    }
    for (int i = 0; i < file_count; i++)
    {
        pthread_join(thread_handles[i], NULL); 
    }
    free(thread_handles);  
    free(original_data); 
    
    return 0; 
}


void save_original_file(revert_pair * original_data, const char * filename, int * file_count)
{
    if (!already_processed(original_data, filename, *file_count))
    {     
        FILE * fptr = fopen(filename, "r"); 
        if (fptr == NULL)
        {
            printf("%sCould not open file:%s %s\n", RED, DEFAULT, filename); 
        }
        else
        {
            copy_file_data(fptr, filename, original_data, *file_count); 
            *file_count += 1; 
            fclose(fptr); 
        }
    }

    return; 
}

int cmp_func(const void * a, const void * b)
{
    int val_a = ((locations*)a)->line_number; 
    int val_b = ((locations*)b)->line_number;  

    return (val_a > val_b) - (val_a < val_b); 
}


void * modify_file (void * modify_data)
{
    FILE * fptr = fopen(((revert_pair*)modify_data)->filename, "r+"); 
    char ** file_data = malloc(1); 

    int line_counter = 0;     
    char buf[512]; 

    int defer_count = 0; 
    locations * defer_locations = malloc(1); 

    while (fgets(buf, sizeof(buf), fptr) != NULL)
    {
        line_counter += 1; 
        file_data = realloc(file_data, sizeof(char *) * line_counter); 
        file_data[line_counter - 1] = malloc(strlen(buf) + 1); 
        strcpy(file_data[line_counter - 1], buf); 
    }

    for (int i = 0; i < line_counter; i++)
    {
        char * pointer; 
        strcpy(buf, file_data[i]); 
        strip_whitespace(buf); 
        pointer = strstr(buf, "defer("); 

        if (pointer == buf)
        {

            int new_location = i + 1; 
            int index = -1; 
            int braces = 0; 
            
            while(braces >= 0 && new_location < line_counter)
            { 
                count_braces(&braces, file_data[new_location]); 
                new_location += 1; 
            }

            for (int x = new_location - 1; x > i; x--)
            {
                pointer = strstr(file_data[x], "return ");
                if (pointer == file_data[x])
                {
                    new_location = x; 
                } 
                else
                {
                    pointer = strstr(file_data[x], "return("); 
                    if (pointer == file_data[x])
                    {
                        new_location = x; 
                    }
                }
            }

            generate_instruction(buf);  

            for (int x = 0; x < defer_count; x++)
            {
                if (defer_locations[x].line_number == new_location)
                {
                    index = x; 
                    break; 
                }
            }

            if (index == -1)
            {
                defer_count += 1; 
                defer_locations = realloc(defer_locations, sizeof(locations) * defer_count); 
                defer_locations[defer_count - 1].instructions = malloc(strlen(buf) + 1); 
                defer_locations[defer_count - 1].line_number = new_location; 
                strcpy(defer_locations[defer_count - 1].instructions, buf);                 
            }
            else
            {
                defer_locations[index].instructions = realloc(defer_locations[index].instructions, strlen(defer_locations[index].instructions) + strlen(buf) + 1); 
                strcat(defer_locations[index].instructions, buf); 
            }

        }

        line_counter += 1; 
    }

    for (int i = 0; i < defer_count; i++)
    {
        int line = defer_locations[i].line_number; 
        strcpy(buf, file_data[line]); 
        file_data[line] = realloc(file_data[line], strlen(defer_locations[i].instructions) + strlen(buf) + 1); 
        strcpy(file_data[line], defer_locations[i].instructions); 
        strcat(file_data[line], buf); 
        free(defer_locations[i].instructions); 
    }    
    free(defer_locations); 

    for (int i = 0; i < line_counter; i++)
    {
        fprintf(fptr, "%s", file_data[i]); 
        free(file_data[i]); 
    }
    free(file_data); 
    fclose(fptr); 

    return NULL; 
}


/*
    Instruction must take the form of:
    defer(function, argument); 
*/
void generate_instruction(char * defer)
{
    char * temp = malloc(strlen(defer) * 2);  

    char function [512] = ""; 
    char argument [512] = ""; 

    // starting at 6 since it starts with defer()
    for (int i = 6; i < (int)strlen(defer); i++)
    {
        while (i < (int)strlen(defer) && temp[i] != ',')
        {
            strcat(function, &defer[i]); 
            i++;    
        }
        while(i < (int)strlen(defer) && temp[i] != ')')
        {
            strcat(argument, &defer[i]); 
            i++; 
        }
    }

    strcpy(temp, function); 
    strcat(temp, "("); 
    strcat(temp, argument); 
    strcat(temp, "); "); 
    strcpy(defer, temp); 
    free(temp); 
    return; 
}


void count_braces(int * brace_counter, const char * line)
{
    for (int i = 0; i < (int)strlen(line); i++)
    {
        if (line[i] == '{')
        {
            *brace_counter += 1; 
        }
        else if (line[i] == '}')
        {
            *brace_counter -= 1; 
        }
    }

    return; 
}

void strip_whitespace(char * string)
{
    char * temp = malloc(strlen(string)); 

    int x = 0; 
    for (int i = 0; i < (int)strlen(string); i++)
    {
        if (!isspace(string[i]))
        {
            temp[x] = string[i]; 
            x++; 
        }
    }
    temp[x] = '\0';

    strcpy(string, temp); 

    return; 
}

void * revert_file (void * revert_data)
{
    FILE * fptr = fopen(((revert_pair*)revert_data)->filename, "w"); 
    fprintf(fptr, "%s", ((revert_pair*)revert_data)->contents); 
    fclose(fptr); 

    free(((revert_pair *)revert_data)->filename); 
    free(((revert_pair *)revert_data)->contents); 

    return NULL;  
}

int already_processed(revert_pair * original_data, const char * filename, int file_count)
{
    for (int i = 0; i < file_count; i++)
    {
        if (strcmp(original_data[i].filename, filename) == 0)
        {
            return 1; 
        } 
    }
    return 0; 
}

void copy_file_data(FILE * fptr, const char * filename, revert_pair * original_data, int index)
{
    int name_len = strlen(filename); 
    int file_len = get_file_size(fptr); 

    original_data = realloc(original_data, sizeof(revert_pair) * (index + 1)); 
    original_data[index].filename = malloc(name_len + 1); 
    original_data[index].contents = malloc(file_len + 1); 

    if (original_data[index].filename == NULL || original_data[index].contents == NULL)
    {
        printf("%sUnable to allocate enough memory. Free up RAM and run program again.\n%s", RED, DEFAULT); 
        exit(1); 
    }

    char * buffer = malloc(file_len + 1); 
    fread(buffer, sizeof(char), file_len, fptr); 

    strcpy(original_data[index].filename, filename); 
    strcpy(original_data[index].contents, buffer);

    free(buffer); 
    return; 
}


int ends_with(const char * string, const char * end)
{
    int len = strlen(string) - 1;  
    int end_len = strlen(end) - 1; 
    
    if (end_len > len)
    {
        return 0; 
    }

    while (end_len >= 0)
    {
        if (string[len] != end[end_len])
        {
            return 0; 
        }
        
        len -= 1; 
        end_len -= 1; 
    }

    return 1; 
}

int get_file_size(FILE * fptr)
{
    fseek(fptr, 0L, SEEK_END); 
    int size = ftell(fptr); 
    fseek(fptr, 0L, SEEK_SET); 
    return size; 
}

