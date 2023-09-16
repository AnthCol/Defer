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
    strncpy(temp, argv[1], strlen(argv[1])); 

    for (int i = 0; i < strlen(temp); i++)
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


void * modify_file (void * modify_data)
{
    FILE * fptr = fopen(((revert_pair*)modify_data)->filename, "r+"); 

    int line_counter = 0;     
    int defer_counter = 0; 
    int return_counter = 0; 
    int start_brace_counter = 0; 
    int end_brace_counter = 0; 

    int * defer_locations = malloc(1); 
    int * return_locations = malloc(1); 
    int * start_brace_locations = malloc(1); 
    int * end_brace_locations = malloc(1); 
    char buf[512]; 

    while (fgets(buf, sizeof(buf), fptr) != NULL)
    {
        line_counter += 1; 
        char * pointer; 
        char temp[512]; 
        strcpy(temp, buf); 

        strip_whitespace(temp);

        if (strncmp(buf, "return ", sizeof("return")) != 0 || strncmp(buf, "return(", sizeof("return")) != 0)
        {   
            return_counter += 1; 
            return_locations = realloc(return_locations, sizeof(int) * return_counter); 
            return_locations[return_counter - 1] = line_counter; 
        }
        else if (strncmp(temp, "defer(", sizeof("defer")) != 0)
        {
            defer_counter += 1;  
            defer_locations = realloc(defer_locations, sizeof(int) * defer_counter); 
            defer_locations[defer_counter - 1] = line_counter; 
        }
        else if (strstr(temp, "}") != NULL)
        { 
            end_brace_counter += 1;  

        }
        else if (strstr(temp, "{") != NULL)
        {            
            start_brace_counter += 1; 

        }
        //line_counter += 1; 
    }



    qsort(defer_locations, defer_counter, sizeof(int), cmp_func); 
    qsort(return_locations, return_counter, sizeof(int), cmp_func); 
    qsort(start_brace_locations, start_brace_counter, sizeof(int), cmp_func); 
    qsort(end_brace_locations, end_brace_counter, sizeof(int), cmp_func); 


    if (start_brace_counter != end_brace_counter)
    {
        printf("%sFile does not have matching number of opening and closing braces (%s)%s\n", RED, (revert_pair*)modify_data->filename, DEFAULT); 
        free(defer_locations); 
        free(return_locations); 
        free(start_brace_locations); 
        free(end_brace_locations); 
        return NULL; 
    }
    
    for (int i = 0; i < start_brace_counter; i++)
    {
        int location = binary_search(end_brace_locations, start_brace_counter, start_brace_locations[i]);

        if (location != -1)
        {
            // FIXME get rid of it 
        }
    } 
    
    //fseek(fptr, 0L, SEEK_SET); 


    /*
        Find the closest scope ending statement for each defer location. 
        Remove defer from the line (leaving only a newline). 

        DEFER MUST BE ON IT'S OWN LINE, AS THE ENTIRE LINE WILL BE DELETED. 

        After, the code will prepend the 
    */


    fclose(fptr); 

    free(defer_locations); 
    free(return_locations); 
    free(end_brace_locations); 

    return NULL; 
}

void strip_whitespace(char * string)
{
    char * temp = malloc(strlen(string)); 

    int x = 0; 
    for (int i = 0; i < strlen(string); i++)
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

int cmp_func(const void * a, const void * b)
{
    return ((*(int*)a) > (*(int*)b)) - ((*(int*)a) < (*(int*)b)); 
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

    strncpy(original_data[index].filename, filename, name_len); 
    strncpy(original_data[index].contents, buffer, file_len);

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

