#include "defer.h"

int main(int argc, char ** argv)
{  

    int err_count = 0;
    char * err_string = malloc(sizeof(char)); 
    char * temp = malloc(sizeof(char) * 100); 

    for (int i = 1; i < argc; i++)
    { 
        if (ends_with(argv[i], "*.c") || ends_with(argv[i], "*.cc")) 
        {
            //multiple_files(argv[i]); 
        }
        else if (ends_with(argv[i], ".c") || ends_with(argv[i], ".cc"))
        {
            //single_file(argv[i]); 
        }
        else
        {
            err_count += 1; 
            sprintf(temp, "%sARGUMENT %d (%s) INVALID. Not a .c or .cc file.\n%s", RED, i, argv[i], DEFAULT); 
            err_string = realloc(err_string, sizeof(char) * (100 * err_count)); 
            strcat(err_string, temp);  
        }
    }
    if (err_count > 1)
    {
        printf("\n\n%sERRORS FROM DEFER:%s\n\n%s\n", PURPLE, DEFAULT, err_string);
        printf("%s", err_string); 
        free(err_string); 
        free(temp); 
        return 0;
    }


    FILE * fptr = fopen(COMPILE_FILE, "r"); 
    unsigned int size = get_file_size(fptr); 
    temp = realloc(temp, size * 2);
    fread(temp, sizeof(char), size, fptr);  
    fclose(fptr); 

    if (strcasestr(temp, " sudo ") != NULL || strcasestr(temp, " rm ") != NULL)
    {
        printf("Compile command contains \"sudo\" and/or \"rm\". Are you sure you want to proceed? (y/n) > "); 
        char c; 
        scanf("%c", &c); 
        while (c != 'y' && c != 'n')
        {
            printf("(y/n) > "); 
            scanf("%c", &c); 
        }
        if (c == 'n')
        {
            free(err_string); 
            free(temp); 
            return 0;   
        }
   }
    
    FILE * compile_process = popen(temp, "r"); 
    char buf [256]; 
    while (fgets(buf, sizeof(buf), compile_process))
    {
        printf("%s", buf); 
    } 
    pclose(compile_process); 
    free(temp); 

    for (int i = 1; i < argc; i++)
    {
        if (ends_with(argv[i], "*.c") || ends_with(argv[i], "*.cc")) 
        {
            revert_multiple(argv[i]); 
        }
        else if (ends_with(argv[i], ".c") || ends_with(argv[i], ".cc"))
        {
            revert_single(argv[i]); 
        } 
    } 

    return 0; 
}

void single_file (const char * file)
{
    FILE * fptr = fopen(file); 
    if (fptr == NULL)
    {
        printf("%sCould not open file:%s%s\n", RED, DEFAULT, file); 
        return; 
    }

    unsigned int size = get_file_size(fptr); 
    char * buffer = malloc(size * 2); 
    fread(buffer, sizeof(char), size, fptr); 

    location_info locations;
    locations.scope = malloc(1); 
    locations.defer = malloc(1); 
    
    find_scope_and_defer(buffer, &locations); 
    substitute_defer(buffer, &locations); 

    free(buffer); 
    free(locations.scope); 
    free(locations.defer); 
    fclose(fptr); 
    return; 
}

void find_scope_and_defer(const char * buffer, location_info * locations)
{
    const char delimiter[2] = "\n"; 
    char * token; 

    
    int * open; 
    int * close; 

    token = strtok(buffer, delimiter); 

    while (token != NULL)
    {
        printf("%s\n", token); 


        token = strtok(NULL, delimiter); 
    }


    return; 
}

void substitute_defer(const char * buffer, location_info * locations)
{

    return; 
}

void multiple_files(const char * files)
{



    return; 
}

void revert_single(const char * file)
{

    return;    
}

void revert_multiple(const char * files)
{

    return; 
}

int ends_with(const char * string, const char * end)
{
    int len = strlen(string); 
    int end_len = strlen(end); 

    if (end_len <= len)
    {
        len -= 1; 
        end_len -= 1; 

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
    
    return 0; 
}

unsigned int get_file_size(FILE * fptr)
{
    unsigned int size; 
    fseek(fptr, 0L, SEEK_END); 
    size = ftell(fptr); 
    fseek(fptr, 0L, SEEK_SET);
    return size; 
}