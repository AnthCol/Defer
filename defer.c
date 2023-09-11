#include "defer.h"

int main(int argc, char ** argv)
{  

    int err_count = 0;
    int file_num = 0; 
    char * err_string = malloc(1); ; 
    char * temp = malloc(100); 

    revert_info * revert = malloc(1); 

    /*
        FIXME
        Make it be able to handle just a star. 
        If the user inputs ** have the program crawl through all the directories 
        and do every single .c or .cc file. 
        This way they won't need to specify each file. 
    */

    for (int i = 1; i < argc; i++)
    {
        if (!ends_with(argv[i], "*.c") && !ends_with(argv[i], "*.cc") && 
            !ends_with(argv[i], ".c")  && !ends_with(argv[i], ".cc"))
        {
            printf("%sARGUMENT %d (%s) INVALID. Not a .c or .cc file.\n%s", RED, i, argv[i], DEFAULT);
        } 
    }

    if (err_count > 0)
    {
        printf("\n%sOnly add C files and run the program again.%s\n", RED, DEFAULT); 
        free(err_string); 
        free(revert);  
        free(temp); 
        return 0; 
    }


    /*
        Revert info here needs to 
        be loaded with each file.
        Both filname as well as the content, so when 
        each file is read it will be loaded. 

    */
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
            /*
                search through directory again and revert files one at a time 
            */
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

    scope_struct scope;  
    scope.pairs = malloc(1); 
    scope.count = 0; 

    int check = find_scopes(buffer, &scope); 

    if (check != 0)
    {
        printf("%sOpening and closing braces do not match in file: %s%s\n", RED, file, DEFAULT); 
        free(buffer); 
        free(scope.pairs); 
        fclose(fptr); 
    }

    find_and_replace_defer(buffer, &scope); 

    free(buffer); 
    free(scope.pairs); 
    fclose(fptr); 
    return; 
}

int find_scopes(const char * buffer, scope_struct * scope)
{
    const char delimiter[2] = "\n"; 
    char * token; 
    char * pointer; 
    int * open = malloc(1);     
    int open_index = 0;  
    int line_number = 1; 

    token = strtok(buffer, delimiter); 

    while (token != NULL)
    {

        if (strstr(token, "{") != NULL)
        {
            open_index += 1; 
            open = realloc(open, sizeof(int) * open_index); 
            open[open_index - 1] = line_number; 
        }   
        else if (strstr(token, "}") != NULL)
        {
            open_index -= 1; 
            scope->count += 1; 
            scope->pairs = realloc(sizeof(pair) * scope->count); 
            scope->pairs[scope->count - 1].first = open[open_index]; 
            scope->pairs[scope->count - 1].second = line_number; 

            open = realloc(open, sizeof(int) * open_index); 
        } 

        line_number += 1; 
        token = strtok(NULL, delimiter);
    }    

    free(open);

    return (open_index == 0); 
}

void find_and_replace_defer(const char * buffer, scope_struct * scope)
{
    const char delimiter[2] = "\n"; 
    char * token; 
    char * pointer; 
    int line_count = 0; 
    int line_number = 1; 
    int ending_line; 

    char ** lines = malloc(1); 

    token = strtok(buffer, delimiter); 

    while (token != NULL)
    {
        line_count += 1; 
        lines = realloc(lines, sizeof(char*) * line_count); 
        lines[line_count - 1] = malloc(sizeof(char) * strlen(token) + 1); 
        strcpy(lines[line_count - 1], token); 
        token = strtok(NULL, delimiter); 
    }

    while(token != NULL)
    {
        strip_whitespace(token); 
        pointer = strstr(token, "defer("); 

        if (pointer != NULL)
        {
            if (check_defer_syntax(pointer))
            {
                ending_line = end_of_scope(line_number, scope);                 
                if (ending_line == -1)
                {
                    printf("%sDefer call on line %d is not within any scope%s\n", RED, line_number, DEFAULT); 
                }
                else
                {
                    
                }
            }
        }

        line_number += 1; 
        token = strtok(NULL, delimiter); 
    }

    for (int i = 0; i < line_count; i++)
    {
        free(lines[i]); 
    } 
    free(lines); 
    return; 
}

int end_of_scope(int line_number, scope_struct * scope)
{
    int index = -1;  
    for (int i = 0; i < scope->count; i++)
    {
        if (line_number > scope->pairs[i].first && 
            line_number < scope->pairs[i].second)
        {
            if (index != -1)
            {
               /*
                Check if it's within a smaller scope
                If it is, update the index variable with 
                the more appropriate index 
               */ 
            }
            else
            {
                index = i; 
            }
        }
    }

    return (index == -1) ? -1 : scope->pairs[index].second; 
}


int check_defer_syntax(const char * defer_token)
{
    for (int i = 0; i < (int)strlen(defer_token); i++)
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
    for (int i = 0; i < (int)strlen(token); i++)
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

void revert_file(const char * filename, revert_info * revert, int file_count)
{
    for (int i = 0; i < file_count; i++)
    {
        if (strcmp(filename, revert[file_count].filename) == 0)
        {
            FILE * fptr = fopen(filename, "w"); 
            fprintf(fptr, "%s", revert[file_count].content); 
            fclose(fptr); 
            break; 
        }
    }

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