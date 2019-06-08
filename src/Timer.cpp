#include "JUtil/Core/Timer.h"
#include "JUtil/Traits/TypeManipulators.hpp"
#ifdef JUTIL_WINDOWS
    #include <windows.h>
#elif defined JUTIL_LINUX
    #include <time.h>
#endif
#include "JUtil/IO/IO.h"

#define NANOS_PER_SECOND 0x3B9ACA00.p0L

namespace jutil JUTIL_PUBLIC_ {


    Timer::Timer() : ct(-1) {
        #ifdef JUTIL_WINDOWS
            LARGE_INTEGER i;
            QueryPerformanceFrequency(&i);
            freq = static_cast<long double>(i.QuadPart) / NANOS_PER_SECOND;
        #else
            freq = 1;
        #endif
    }
    Timer::Timer(const Timer &timer)
    #ifdef JUTIL_CPP11
        : Timer() {
            *this = timer;
        }
    #else
        : ct(timer.ct) {
            #ifdef JUTIL_WINDOWS
                LARGE_INTEGER i;
                QueryPerformanceFrequency(&i);
                freq = static_cast<long double>(i.QuadPart) / NANOS_PER_SECOND;
            #else
                freq = 1;
            #endif
        }
    #endif
    
    Timer &Timer::operator=(const Timer &timer) {
        ct = timer.ct;
        return *this;
    }
    #ifdef JUTIL_CPP11
        Timer::Timer(Timer &&timer) : Timer() {
            *this = jutil::move(timer);
        }
        Timer &Timer::operator=(Timer &&timer) {
            ct = timer.ct;
            timer.ct = -1;
            return *this;
        }
    #endif

    long long queryGlobalTimer() {
        #ifdef JUTIL_WINDOWS
            LARGE_INTEGER i;
            QueryPerformanceCounter(&i);
            return i.QuadPart;
        #elif defined JUTIL_LINUX
            timespec t;
            clock_gettime(CLOCK_REALTIME, &t);
            return (t.tv_sec * NANOS_PER_SECOND) + t.tv_nsec;
        #endif
    }

    void Timer::start() {
        ct = queryGlobalTimer();
    }
    long double Timer::get(unsigned type) {
        long long nct = queryGlobalTimer();
        long double transform = freq;
        for (unsigned i = 0; i < type; ++i) {
            transform *= 1000.L;
        }
        return static_cast<long double>(nct - ct) / transform;
    }
    long double Timer::stop(unsigned type) {
        long double r = get(type);
        ct = -1;
        return r;
    }
    long double Timer::fastForward(long double t, unsigned type) {
        for (unsigned i = 0; i < type; ++i) t *= 1000.L;
        ct -= (t * freq);
        return get(type);
    }

    long double Timer::rewind(long double t, unsigned type) {
        for (unsigned i = 0; i < type; ++i) t *= 1000.L;
        ct += (t * freq);
        return get(type);
    }

    long double Timer::restart(unsigned type) {
        long double r = stop(type);
        start();
        return r;
    }
    void Timer::set(long double t, unsigned type) {
        long double cur = get(type);
        long double diff = t - cur;
        if (diff < 0) rewind(diff * -1, type);
        else fastForward(diff, type);
    }
}
