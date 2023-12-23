#include "defer.h"

int main(int argc, char ** argv)
{

    flag_container flags = {false, false, false, false};
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
        pthread_create(&thread_handles[i], NULL, (flags.backwards) ? backwards : forwards, (void *) &c_files[i]); 
 
    for (int i = 0; i < (int)c_files.size(); i++)
        pthread_join(thread_handles[i], NULL); 

    delete[] thread_handles; 

    return 0; 
}


void * forwards(void * data)
{
    std::string * file = static_cast<std::string *>(data); 
    std::fstream fptr (*file, std::ios::in); 
    std::vector<std::string> file_data = read_file(fptr);  
    fptr.close(); 
    
    if (uneven_braces(file_data))
    {
        std::cout << "File named: " << *file << " cannot be processed. Unequal brace {} count\n"; 
        return NULL; 
    }

    
    modify_file_info(file_data); 

    std::cout << "PRINTING FILE DATA: " << std::endl; 


    for (std::string s : file_data)
        std::cout << s << std::endl; 


    /*
    fptr.open(*file, std::ios::out); 
    write_to_file(file_data, fptr); 
    */

    return NULL; 
}

void * backwards(void * data)
{
    std::string * file = static_cast<std::string *>(data);  
    std::fstream fptr (*file, std::ios::in); 
    std::vector<std::string> file_data = read_file(fptr); 
    fptr.close(); 

    /*
    revert_file_info(file_data); 
    fptr.open(*file, std::ios::out); 
    write_to_file(file_data, fptr); 
    */

    return NULL; 
}

void modify_file_info(std::vector<std::string>& file_data)
{
    std::string prefix = "#defer"; 
    std::string instruction; 

    for (int i = file_data.size() - 1; i >= 0; i--)
        if (starts_with(file_data[i], prefix))
            defer(file_data, i); 

    return; 
}

bool starts_with(std::string& text, std::string& prefix)
{
    std::string temp = text; 
    temp.erase(0, temp.find_first_not_of(" \t")); 
    return(temp.compare(0, prefix.length(), prefix) == 0); 
}


void defer(std::vector<std::string>&file_data, int index)
{
    std::string instruction = file_data[index].substr(file_data[index].find(" ") + 1); 
    instruction.erase(0, instruction.find_first_not_of(" \t")); 
    instruction.erase(0, instruction.find("#defer") + std::string("#defer").length()); 
    instruction.erase(0, instruction.find_first_not_of(" \t")); 

 
    file_data[index] = "//" + file_data[index]; 
    return; 
}

void revert()
{
    return; 
}

void revert_file_info(std::vector<std::string>& file_data)
{

}


void write_to_file(std::vector<std::string>& file_data, std::fstream& fptr)
{ 
    fptr << std::accumulate(file_data.begin(), file_data.end(), std::string("")); 
    return; 
}



std::vector<std::string> read_file(std::fstream& fptr)
{
    std::vector<std::string> file_data; 
    std::string s; 
    while (getline(fptr, s))
        file_data.push_back(s); 
    return file_data; 
}

bool uneven_braces(std::vector<std::string>& file_data)
{
    int brace_count = 0;
    for (std::string s : file_data)
        for (char c : s)
            if (c == '{')
                brace_count += 1;
            else if (c == '}')
                brace_count -= 1; 
    return (brace_count != 0);  
}

void determine_flags(flag_container& flags, int argc, char ** argv)
{
    for (int i = 1; i < argc; i++)
        set_flag(flags, std::string(argv[i])); 
    return; 
}

void set_flag(flag_container& flags, std::string arg)
{  
    if (arg == "-help" || arg == "-h" || arg == "-H")
        flags.help = true; 
    else if (arg == "-r" || arg == "-R")
        flags.recursive = true; 
    else if (arg == "-b" || arg == "-B")
        flags.backwards = true; 
    else if (arg == "-v" || arg == "-V")
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
    int i = suffix.length() - 1; 
    int j = text.length() - 1; 
    while (i >= 0)
        if (suffix[i--] != text[j--])
            return false; 
    return true; 
}

void print_help()
{
    std::string help_info = "\nDEFER HELP MENU\n\n"; 
    help_info += "[ Flags ]\n\n"; 
    help_info += "-r     =>  Will recursively search and use all C files from current and all sub-directories\n";  
    help_info += "-R     =>  Will recursively search and use all C files from current and all sub-directories\n\n"; 
    help_info += "-b     =>  Will revert files that have previously gone through this program back into normal C code\n"; 
    help_info += "-B     =>  Will revert files that have previously gone through this program back into normal C code\n\n"; 
    help_info += "-v     =>  Will print the version number of the program\n"; 
    help_info += "-V     =>  Will print the version number of the program\n\n"; 
    help_info += "-h     =>  Will print this help menu\n"; 
    help_info += "-H     =>  Will print this help menu\n"; 
    help_info += "-help  =>  Will print this help menu\n";
    std::cout << help_info << std::endl; 
    return; 
}

void print_version()
{
    std::cout << "Defer version 1.0\n"; 
    std::cout << "Written by Anthony Colaiacovo in 2023" << std::endl; 
    return; 
}

