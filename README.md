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

