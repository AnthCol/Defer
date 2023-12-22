#include "defer.h"

int main(int argc, char ** argv)
{

    flag_container flags; 
    std::vector<std::string> c_files; 
    
    determine_flags(flags, argc, argv); 

    if (flags.help)
    {
        print_help(); 
        return 0; 
    }

    if (flags.version)
    {
        print_version(); 
        return 0; 
    }


    if (flags.recursive)
        recursive_get_c_files(c_files); 
    else
        get_c_files(c_files, argc, argv); 

    

    pthread_t * thread_handles = new pthread_t[c_files.size()]; 

    for (int i = 0; i < (int)c_files.size(); i++)
        pthread_create(&thread_handles[i], NULL, (flags.backwards) ? backwards : forwards, &c_files[i]); 
 
    for (int i = 0; i < (int)c_files.size(); i++)
        pthread_join(thread_handles[i], NULL); 

    delete[] thread_handles; 

    return 0; 
}


void * forwards(void * data)
{
    std::string * file = static_cast<std::string *>(data); 



    return NULL; 
}

void * backwards(void * data)
{
    std::string * file = static_cast<std::string *>(data); 



    return NULL; 
}


void determine_flags(flag_container& flags, int argc, char ** argv)
{
    for (int i = 1; i < argc; i++)
        set_flag(flags, std::string(argv[i])); 
    return; 
}

void set_flag(flag_container& flags, std::string arg)
{  
    if (arg == "-help" || arg == "-h")
        flags.help = true; 
    else if (arg == "-r")
        flags.recursive = true; 
    else if (arg == "-b")
        flags.backwards = true; 
    else if (arg == "-v")
        flags.version = true; 
    return; 
}

void recursive_get_c_files(std::vector<std::string>& c_files)
{
    std::filesystem::path cwd = std::filesystem::current_path(); 
    std::filesystem::recursive_directory_iterator rdi (cwd); 
    for (const auto& file : rdi)
        if (ends_with(file.path().filename().string(), std::string(".c")))
            c_files.push_back(file.path()); 
    return; 
}

void get_c_files(std::vector<std::string>& c_files, int argc, char ** argv)
{
    for (int i = 1; i < argc; i++)
        if (ends_with(std::string(argv[i]), std::string(".c")))
            c_files.push_back(std::string(argv[i])); 
    return; 
}

bool ends_with(std::string text, std::string suffix)
{
    int i = suffix.length(); 
    int j = text.length(); 
    while (i >= 0)
        if (suffix[i--] != text[j--])
            return false; 
    return true; 
}

void print_help()
{
    std::string help_info = "\n Defer Help Menu\n"; 
    help_info += "[ Flags ]\n"; 
    help_info += "-r    =>  Will recursively search and use all C files from current and all sub-directories\n"; 
    help_info += "-b    =>  Will revert files that have previously gone through this program back into normal C code\n"; 
    help_info += "-v    =>  Will print the version number of the program\n"; 
    help_info += "-h    =>  Will print this help menu\n"; 
    help_info += "-help =>  Will print this help menu\n";
    std::cout << help_info << std::endl; 
    return; 
}

void print_version()
{
    std::cout << "Defer version 1.0\n"; 
    std::cout << "Written by Anthony Colaiacovo, 2023" << std::endl; 
    return; 
}

