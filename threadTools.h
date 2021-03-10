#pragma once
//these are tools and function to be used for threadding in this project
#ifdef __GNUC__
    //for mingw/gcc
    #include <pthread.h>


#endif
#ifdef win64
    //for win64 (VS)
    #include <winpthreads.h>
#endif