#ifndef SIZE_T_H_INCLUDED
#define SIZE_T_H_INCLUDED

#ifndef _WIN64
    #if __cplusplus >= 201103L
        #define JUTIL_SIZE_T    decltype(alignof(char))
    #else
        #define JUTIL_SIZE_T    unsigned
    #endif
#else
    #define JUTIL_SIZE_T        unsigned long long
#endif

#if !(defined(size_t)) && !defined(_SIZE_T_DEFINED) && !defined(_SIZE_T_DEFINED_)
    #undef size_t
    #define _SIZE_T_DEFINED
    #define _SIZE_T_DEFINED_
    typedef JUTIL_SIZE_T size_t;
#endif

#endif // SIZE_T_H_INCLUDED
