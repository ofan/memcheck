#Summary
This module overloaded the default new & delete operators. And it can help programmer to trace memory allocating & freeing.

#Features
* Tracing memory allocating & freeing with giving useful information.
* Automatically check memory leak and give warnings when the program exits.
* Checking memory leak manually on runtime.
* The project is in Public Domain.

#Usage
1. Include "memcheck.h" where you want to check memory leak.
2. Compile your sources & memcheck.cpp
3. Link memcheck.cpp with your program

_NOTE:_"memcheck.h" must be included after other standard headers

```cpp
    #include <vector>
    #include "memcheck.h"//Right

    /******************************/

    #include "memcheck.h" //WRONG!
    #include <vector>
```

#Contribution
Currently this is just a toy project, it's not even thread-safe.
Any kinds of contribution is welcome!
