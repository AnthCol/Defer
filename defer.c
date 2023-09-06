#include "defer.h"

int main(int argc, char ** argv)
{  

    int err_count = 0;
    char * err_string = malloc(sizeof(char)); 
    char * temp = malloc(sizeof(char) * 100); 

    for (int i = 1; i < argc; i++)
    { 
        if (ends_with_star_c(argv[i]))
        {
            /*
                Go into system and generate file list of all the .c files in that directory. 
                Add them to a list and process one by one. 
            */
        }
        else if (ends_with_c(argv[i]))
        {

           /*
                Open and parse that specific file.  
           */ 
        }
        else
        {
            err_count += 1; 
            sprintf(temp, "%sARGUMENT %d (%s) INVALID. Not a .c or .cc file.\n%s", RED, i, argv[i], DEFAULT); 
            err_string = realloc(err_string, sizeof(char) * (100 * err_count)); 
            strcat(err_string, temp);  
        }
    }


    FILE * fptr = fopen(COMPILE_FILE, "r"); 

     


    fclose(fptr);



    printf("\n\nErrors from defer:\n%s", err_string);  
    
    free(err_string); 
    free(temp); 

    return 0; 
}


int ends_with_c(const char * string)
{   
    int len = strlen(string);  

    if (len < 3)
    {
        return 0;
    }
    else
    {
        int test_1 = (string[len - 2] == '.' && string[len - 1] == 'c'); 
        int test_2 = 0; 
        
        if (len > 3)
        {
            (string[len - 3] == '.' && string[len - 2] == 'c' && string[len - 1] == 'c');  
        }
        
        return (test_1 || test_2);      
    }
}

int ends_with_star_c(const char * string)
{
    int len = strlen(string); 
    
    if (len < 3)
    {
        return 0; 
    }
    else
    {
        int test_1 = (string[len - 3] == '*' && string[len - 2] == '.' && string[len - 1] == 'c'); 
        int test_2 = 0; 

        if (len > 3)
        {
            test_2 = (string[len - 4] == '*' && string[len - 3] == '.' && string[len - 2] == 'c' && string[len - 1] == 'c'); 
        }

        return (test_1 || test_2); 
    }
}

