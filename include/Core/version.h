#ifndef JUTIL_VERSION_H
#define JUTIL_VERSION_H

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

    #define JUTIL_VERSION 1.4

    #ifdef _WIN32
        #define JUTIL_WINDOWS
    #elif defined(linux) || defined(__linux)
        #define JUTIL_LINUX
    #endif

    #ifdef __GNUC__
        #ifdef __GNUC_PATCHLEVEL__
            #define JUTIL_GCC __JUTIL_CONCAT(__GNUC__, __JUTIL_CONCAT(., __JUTIL_CONCAT(__GNUC_MINOR__, __GNUC_PATCHLEVEL__)))
        #else
            #define JUTIL_GCC __JUTIL_CONCAT(__GNUC__, __JUTIL_CONCAT(., __GNUC_MINOR__))
        #endif
    #endif

    #ifdef __MINGW32__
        #include <_mingw_mac.h>
        #define JUTIL_MINGW __JUTIL_CONCAT(__MINGW32_MAJOR_VERSION, __JUTIL_CONCAT(., __MINGW32_MINOR_VERSION))
        #ifdef __MINGW64__
            #define JUTIL_MINGW64 __JUTIL_CONCAT(__MINGW64_VERSION_MAJOR, __JUTIL_CONCAT(., __MINGW64_VERSION_MINOR))
        #endif
    #endif

    #ifdef _MSC_VER
        #define JUTIL_MSVC _MSC_VER
    #endif

    #if !defined(JUTIL_WINDOWS) && !defined(JUTIL_LINUX)
        #error Unsupported operating system! JUtil reuires windows or linux to compile.
    #endif

    #if !defined(JUTIL_GCC) && !defined(JUTIL_MSVC)
        #error Unsupported compiler! Please use GCC or MSVC.
    #endif

#else
    #error JUtil requires C++ to compile!

#endif // __cplusplus

#endif // JUTIL_VERSION_H
