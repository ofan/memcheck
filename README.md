This module overloaded the default new & delete operators.And it can help programmer to trace memory allocating & freeing.

Features:

Tracing memory allocating & freeing with giving useful information.
Automatically check memory leak and give warnings when the program exits.
Checking memory leak manually on runtime.
The project is in Public Domain.
How to use:

Include "memcheck.h" where you want to check memory leak.
Compile your sources & memcheck.cpp
Link memcheck.cpp with your program
NOTE:"memcheck.h" must be included after other system headers:
/*{{{
#include <vector>
#include "memcheck.h"//Right

/******************************/

#include "memcheck.h" //WRONG!
#include <vector>
It will be appreciate if you give me some feedbacks.
  }}}*/

Email: odayfans@gmail.com
