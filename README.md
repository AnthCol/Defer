## defer

# Purpose
This project is effectively an extra compiler pass that allows you to use the "defer"
feature that is found in other languages, in C. 

# How to use
The header file contains the following:
```
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
```
so you must make sure you have a machine capable of using these things. 

If you do, use your compiler to compile the defer.c file into an executable (makefile is provided), for example:
```
gcc defer.c -o defer
```

Then, run the program with C files as command line arguments. For example:
```
./defer temp.c ../test.c folder/*.c
```
# How does this work?

# Future updates

<ul>
    <li>Currently it will only work with functions that take singular arguments. Add support for multi-argument functions in the future. </li>
</ul>

# Flaws

One of the major flaws (although not one that would come up often if one was adhering to good
code style) is that the program will not be able to handle something like this:
```
defer(free, arr1); defer(free, arr2); printf("test\n"); defer(free, arr3); 
```
For this program to work correctly, the defer "function call" must be on it's own line. 


It currently is also unable to handle something like this:
```
for (int i = 0; i < 10; i++) { if (i < 5) { printf("%s\n", temp); }}
```
In short, braces on the same line will not work because of the way I chose to parse the lines. 
It will work fine provided opening and closing braces are all on different lines.
