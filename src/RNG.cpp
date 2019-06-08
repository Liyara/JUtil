#include "JUtil/Core/RNG.h"
#include "JUtil/Traits/TypeManipulators.hpp"
#include <time.h>
#ifdef JUTIL_WINDOWS
    #include <windows.h>
#endif

#define JUTIL_RNG_MAX __DBL_MAX__
#define JUTIL_RNG_MIN (-__DBL_MAX__ - 1)

namespace jutil {

    int64_t queryEntropicSources(void** time) {
        #ifdef JUTIL_WINDOWS
            *time = (void*) new LARGE_INTEGER;
            QueryPerformanceCounter((LARGE_INTEGER*)*time);
            return ((LARGE_INTEGER*)*time)->QuadPart;
        #elif defined JUTIL_LINUX
            *time = (void*) new timespec;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, (timespec*)*time);
            return ((timespec*)*time)->tv_sec + ((timespec*)*time)->tv_nsec;
        #endif
    }

    RNG::RNG() 
    #ifdef JUTIL_CPP11
        : RNG(JUTIL_RNG_MIN + 1, JUTIL_RNG_MAX - 1) {}
    #else
        : floor(JUTIL_RNG_MIN + 1), ceiling(JUTIL_RNG_MAX - 1) {
            internalValue = queryEntropicSources(&_time);
        }
    #endif

    RNG::RNG(long double f, long double c) : floor(f), ceiling(c) {
        internalValue = queryEntropicSources(&_time);
    }

    RNG::RNG(const RNG &o) {
        (*this) = o;
    }
    RNG &RNG::operator=(const RNG &o) {
        floor = o.floor;
        ceiling = o.ceiling;
        internalValue = o.internalValue;
        return *this;
    }
    #ifdef JUTIL_CPP11
        RNG::RNG(RNG &&o) {
            (*this) = jutil::move(o);
        }
        RNG &RNG::operator=(RNG &&o) {
            floor = o.floor;
            ceiling = o.ceiling;
            internalValue = o.internalValue;
            o.floor = 0;
            o.ceiling = 0;
            o.internalValue = 0;
            return *this;
        }
    #endif

    void RNG::setFloor(long double f) {
        floor = f;
    }

    void RNG::setCeiling(long double c) {
        ceiling = c;
    }

    void RNG::shake() {
        #ifdef JUTIL_WINDOWS
            QueryPerformanceCounter((LARGE_INTEGER*)_time);
        #elif defined JUTIL_LINUX
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, (timespec*)_time);
        #endif
            
        internalValue ^= internalValue >> 12;
        internalValue ^= internalValue << 25;
        internalValue ^= internalValue >> 27;

        #ifdef JUTIL_WINDOWS
            internalValue *= ((LARGE_INTEGER*)_time)->QuadPart;
        #elif defined JUTIL_LINUX
            internalValue *= ((timespec*)_time)->tv_sec + ((timespec*)_time)->tv_nsec;
        #endif
    }
}
