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

            printf("first path taken\n"); 
            /*
                Go into system and generate file list of all the .c files in that directory. 
                Add them to a list and process one by one. 
            */
        }
        else if (ends_with(argv[i], ".c") || ends_with(argv[i], ".cc"))
        {

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
    fseek(fptr, 0L, SEEK_END); 
    unsigned int size = ftell(fptr); 
    temp = realloc(temp, size * 2); 
    fseek(fptr, 0L, SEEK_SET); 
    fread(temp, sizeof(char), size, fptr);  
    fclose(fptr); 

    if (strcasestr(temp, "sudo") != NULL || strcasestr(temp, "rm") != NULL)
    {
        printf("Compile command contains \"sudo\" and/or \"rm\", exiting program for safety reasons.\n"); 
        free(err_string); 
        free(temp); 
        return 0;  
    }
    
    FILE * compile_process = popen(temp, "r"); 
    char buf [256]; 
    while (fgets(buf, sizeof(buf), compile_process))
    {
        printf("%s", buf); 
    } 
    pclose(compile_process); 
    free(temp); 


    return 0; 
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

