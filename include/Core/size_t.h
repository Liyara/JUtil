#ifndef SIZE_T_H_INCLUDED
#define SIZE_T_H_INCLUDED

#if __cplusplus >= 201103L
    #define JUTIL_SIZE_T    decltype(alignof(char))
#else
    #define JUTIL_SIZE_T    unsigned
#endif

#if !(defined(size_t))
    typedef JUTIL_SIZE_T size_t;
#endif

#endif // SIZE_T_H_INCLUDED
