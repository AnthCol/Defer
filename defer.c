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
    int size = get_file_size(fptr); 
    char * file_data = malloc(size + 1); 
    fread()

    int line_counter = 0;     
    char buf[512]; 

    int defer_count = 0; 
    locations * defer_locations = malloc(1); 

    while (fgets(buf, sizeof(buf), fptr) != NULL)
    {
        line_counter += 1; 
        char temp[512]; 
        char formatted[512]; 
        strcpy(temp, buf); 
        strip_whitespace(temp);

       
        if (strncmp(temp, "defer(", sizeof("defer")) == 0)
        {
            // save line
            strcpy(formatted, temp); 


            // wipe line in file. 



            int end_location = line_counter; 
            int brace_counter = 0; 
            int ending_location; 

            while (brace_counter >= 0)
            {
                fgets(buf, sizeof(buf), fptr); 
                end_location += 1; 

                for (int i = 0; i < strlen(buf); i++)
                {
                    if (buf[i] == '{')
                    {
                        brace_counter += 1; 
                    }
                    else if (buf[i] == '}')
                    {
                        brace_counter -= 1; 
                    }
                }
            }

            /*
                FORMAT TEMP INSTRUCTION
            */

            int found_index = -1; 

            for (int i = 0; i < defer_count; i++)
            {
                if(defer_locations[i].line_number == end_location)
                {
                    found_index = i; 
                    break; 
                }
            }

            if (found_index == -1)
            {
                defer_count += 1; 
                defer_locations = realloc(defer_locations, sizeof(location) * defer_count); 
                defer_locations[defer_count - 1].instructions = malloc(strlen(formatted) + 1); 
                defer_locations[defer_count - 1].line_number = end_location; 
                
                strcpy(defer_locations[defer_count - 1].instructions, formatted); 
            }
            else
            {
                defer_locations[found_index].instructions = realloc(defer_locations[found_index].instructions, strlen(defer_locations[found_index].instructions) + strlen(formatted) + 1); 
                strcat(defer_locations[found_index].instructions, formatted); 
            }

        }
    
        // Set file back to where we started at
        fseek(fptr, 0L, SEEK_SET); 
        int temp_counter = 0; 
        while (line_counter != temp_counter)
        {
            fgets(buf, sizeof(buf), fptr); 
        }
    }

    // Sort structs by their ending locations 
    qsort(defer_locations, defer_count, sizeof(locations), cmpfunc); 

    fseek(fptr, 0L, SEEK_SET); 
    line_counter = 0; 

    for (int i = 0; i < defer_count; i++)
    {
        while (line_counter != defer_locations[i].line_number)
        {
            fgets(buf, sizeof(buf), fptr); 
            line_counter += 1; 
        }
        



        free(defer_locations[i].instructions); 
    }
    free(defer_locations); 
    fclose(fptr); 

    return NULL; 
}


void count_braces(int * brace_counter, const char * line)
{
    for (int i = 0; i < strlen(line); i++)
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

    if (end_len < len)
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

