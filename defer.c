#include "defer.h"

int main(int argc, char ** argv)
{  

    int err_count = 0;
    int file_num = 0; 
    char * err_string = malloc(1); ; 
    char * temp = malloc(100); 

    revert_info * revert = malloc(1); 

    for (int i = 1; i < argc; i++)
    { 
        if (ends_with(argv[i], "*.c") || ends_with(argv[i], "*.cc")) 
        {
            /*
                Loop and call single file. 

            
            */
        }
        else if (ends_with(argv[i], ".c") || ends_with(argv[i], ".cc"))
        {
            file_num += 1; 
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
            revert_multiple(argv[i], revert, file_num); 
        }
        else if (ends_with(argv[i], ".c") || ends_with(argv[i], ".cc"))
        {
            revert_single(argv[i], revert, file_num); 
        } 
    } 


    for (int i = 0; i < file_num; i++)
    {
        free(revert[i].filename); 
        free(revert[i].content); 
    }
    free(revert); 

    return 0; 
}

void modify_file(const char * file, revert_info * revert, int file_num)
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

    revert = realloc(revert, sizeof(revert_info) * file_num); 
    revert[file_num - 1].filename = malloc(strlen(file) + 1); 
    revert[file_num - 1].content = malloc(size + 1); 
    strcpy(revert[file_num - 1].filename, file); 
    strcpy(revert[file_num - 1].content, buffer); 


    pair * scopes = malloc(1); 

    find_scopes(buffer, scopes); 
    find_and_replace_defer(buffer, scopes); 

    free(buffer); 
    free(scopes); 
    fclose(fptr); 
    return; 
}

void find_scopes(const char * buffer, pair * scopes)
{
    const char delimiter[2] = "\n"; 
    char * token; 
    char * pointer; 
    int line_number = 1; 

    token = strtok(buffer, delimiter); 

    while (token != NULL)
    {

        line_number += 1; 
        token = strtok(NULL, delimiter);
    }    

    return; 
}

void find_and_replace_defer(const char * buffer, pair * scopes)
{
    const char delimiter[2] = "\n"; 
    char * token; 
    char * pointer; 
    int line_number = 1; 

    token = strtok(buffer, delimiter); 

    while(token != NULL)
    {
        strip_whitespace(token); 
        pointer = strstr(token, "defer("); 

        if (pointer != NULL)
        {
            if (check_defer_syntax(pointer))
            {
                /*
                    FIXME
                    Search through scopes and swap out buffer where necessary. 
                */
            }
        }

        line_number += 1; 
        token = strtok(NULL, delimiter); 
    }

    return; 
}

int check_defer_syntax(const char * defer_token)
{

    for (int i = 0; i < strlen(defer_token); i++)
    {

    }

    return 1; 
}

void strip_whitespace(char * token)
{
    char * temp = malloc(strlen(token) + 1); 
    if (temp == NULL)
    {
        printf("Malloc fail. Exiting...\n"); 
        exit(1); 
    }

    int x = 0; 
    for (int i = 0; i < strlen(token); i++)
    {
        if (!isspace(token[i]))
        {
            temp[x] = token[i]; 
            x += 1; 
        }
    }
    temp[x] = '\0'; 

    strcpy(token, temp); 
    free(temp); 
    return; 
}

void revert_single(const char * file)
{

    return;    
}

int ends_with(const char * string, const char * end)
{
    int len = strlen(string) - 1; 
    int end_len = strlen(end) - 1; 

    if (end_len <= len)
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

unsigned int get_file_size(FILE * fptr)
{
    unsigned int size; 
    fseek(fptr, 0L, SEEK_END); 
    size = ftell(fptr); 
    fseek(fptr, 0L, SEEK_SET);
    return size; 
}