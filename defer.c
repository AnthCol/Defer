#include "defer.h"

 
int main(int argc, char ** argv)
{
    int size; 
    int file_count = 0;
    char buf [512]; 
    FILE * fptr; 
    char * temp = malloc(1);
    revert_pair * original_data = malloc(1); 

    if (strcmp(argv[1], "all") == 0 || strcmp(argv[1], "ALL") == 0)
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
            save_original_file(original_data, argv[i], &file_count); 
        }
    }

    printf("yo\n"); 
    exit(1); 

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
    free(temp); 
    
    
    for (int i = 0; i < file_count; i++)
    {
        pthread_create(&thread_handles[i], NULL, revert_file, (void *) &original_data[i]); 
        free(original_data[i].filename); 
        free(original_data[i].contents); 
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



    return NULL; 
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