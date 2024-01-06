#ifndef JUTIL_TIMER_H
#define JUTIL_TIMER_H

#include "JUtil/Traits/TypeManipulators.hpp"
#ifdef JUTIL_WINDOWS
    #include <windows.h>
#elif defined JUTIL_LINUX
    #include <time.h>
#endif
#include "JUtil/IO/IO.h"

#define NANOS_PER_SECOND 0x3B9ACA00.p0L

namespace jutil  {

    inline long long queryGlobalTimer() {
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

    enum {
        NANOSECONDS,
        MICROSECONDS,
        MILLISECONDS,
        SECONDS
    };

    class  Timer {
    public:

        Timer() : ct(-1) {
            #ifdef JUTIL_WINDOWS
                LARGE_INTEGER i;
                QueryPerformanceFrequency(&i);
                freq = static_cast<long double>(i.QuadPart) / NANOS_PER_SECOND;
            #else
                freq = 1;
            #endif
        }
        Timer(const Timer &timer)
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
        
        Timer &operator=(const Timer &timer) {
            ct = timer.ct;
            return *this;
        }
        #ifdef JUTIL_CPP11
            Timer(Timer &&timer) : Timer() {
                *this = move(timer);
            }
            Timer &operator=(Timer &&timer) {
                ct = timer.ct;
                timer.ct = -1;
                return *this;
            }
        #endif

       void start() {
            ct = queryGlobalTimer();
        }
        long double get(unsigned type) {
            long long nct = queryGlobalTimer();
            long double transform = freq;
            for (unsigned i = 0; i < type; ++i) {
                transform *= 1000.L;
            }
            return static_cast<long double>(nct - ct) / transform;
        }
        long double stop(unsigned type = MILLISECONDS) {
            long double r = get(type);
            ct = -1;
            return r;
        }
        long double fastForward(long double t, unsigned type) {
            for (unsigned i = 0; i < type; ++i) t *= 1000.L;
            ct -= (t * freq);
            return get(type);
        }

        long double rewind(long double t, unsigned type) {
            for (unsigned i = 0; i < type; ++i) t *= 1000.L;
            ct += (t * freq);
            return get(type);
        }

        long double restart(unsigned type) {
            long double r = stop(type);
            start();
            return r;
        }
        void set(long double t, unsigned type) {
            long double cur = get(type);
            long double diff = t - cur;
            if (diff < 0) rewind(diff * -1, type);
            else fastForward(diff, type);
        }

    private:
        long long ct;
        long double freq;
    };

    
}

#endif // JUTIL_TIMER_H
