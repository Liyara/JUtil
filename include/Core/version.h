#ifndef JUTIL_VERSION_H
#define JUTIL_VERSION_H

#if __cplusplus >= 201103L
    #define JUTIL_CPP11
#endif

#if __cplusplus >= 201402L
    #define JUTIL_CPP14
#endif

#if __cplusplus >= 201703L
    #define JUTIL_CPP17
#endif

#define JUTIL_VERSION 1.3

#endif // JUTIL_VERSION_H
