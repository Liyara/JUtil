#ifndef JUTIL_VERSION_H
#define JUTIL_VERSION_H

#define CORE_ERR_INDEX      0x00
#define VERSION_ERR_INDEX   0x00

#define __JUTIL_CONCAT(a, b) __JUTIL_CONCAT_(a, b)
#define __JUTIL_CONCAT_(a, b) a##b

#ifdef __cplusplus

    #define JUTIL_CPP __cplusplus

    #if JUTIL_CPP < 201103L
        #define JUTIL_CPP98
    #endif

    #if JUTIL_CPP >= 201103L
        #define JUTIL_CPP11
    #endif

    #if JUTIL_CPP >= 201402L
        #define JUTIL_CPP14
    #endif

    #if JUTIL_CPP >= 201703L
        #define JUTIL_CPP17
    #endif

    #define JUTIL_VERSION 1.6

    #if _WIN32 || _WIN64
        #if _WIN64
            #define JUTIL_BITS  64
        #else
            #define JUTIL_BITS  32
        #endif
        #define JUTIL_WINDOWS
    #elif linux || __linux
        #define JUTIL_LINUX
    #endif

    #if __GNUC__
        #ifndef JUTIL_BITS
            #if __x86_64__ || __ppc64__
                #define JUTIL_BITS  64
            #else
                #define JUTIL_BITS  32
            #endif
        #endif
        #if __GNUC_PATCHLEVEL__
            #define JUTIL_GCC __JUTIL_CONCAT(__GNUC__, __JUTIL_CONCAT(., __JUTIL_CONCAT(__GNUC_MINOR__, __GNUC_PATCHLEVEL__)))
        #else
            #define JUTIL_GCC __JUTIL_CONCAT(__GNUC__, __JUTIL_CONCAT(., __GNUC_MINOR__))
        #endif
    #endif

    #if __MINGW32__
        #define JUTIL_MINGW __JUTIL_CONCAT(__MINGW32_MAJOR_VERSION, __JUTIL_CONCAT(., __MINGW32_MINOR_VERSION))
        #define JUTIL_MINGW_32
        #if __MINGW64__
            #include <_mingw_mac.h>
            #define JUTIL_MINGW64 __JUTIL_CONCAT(__MINGW64_VERSION_MAJOR, __JUTIL_CONCAT(., __MINGW64_VERSION_MINOR))
            #define JUTIL_MINGW_64
        #endif
    #endif

    #if _MSC_VER
        #define JUTIL_MSVC _MSC_VER
    #endif

    #if !defined(JUTIL_WINDOWS) && !defined(JUTIL_LINUX)
        #error Unsupported operating system! JUtil reuires windows or linux to compile.
    #endif

    #if !defined(JUTIL_GCC) && !defined(JUTIL_MSVC)
        #error Unsupported compiler! Please use GCC or MSVC.
    #endif

    #if !defined(JUTIL_BITS)
        #error Unsupported architecture! Please use x86 or x86_64 architectrure!
    #endif

#else
    #error JUtil requires C++ to compile!
#endif // __cplusplus

#endif // JUTIL_VERSION_H
