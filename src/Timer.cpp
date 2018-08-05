#include "Core/Timer.h"
#include "Traits/TypeManipulators.hpp"
#include <windows.h>
#include "IO/IO.h"

#define NANOS_PER_SECOND 0x3B9ACA00.p0L

namespace jutil JUTIL_PUBLIC_ {


    Timer::Timer() : ct(-1) {
        LARGE_INTEGER i;
        QueryPerformanceFrequency(&i);
        freq = static_cast<long double>(i.QuadPart) / NANOS_PER_SECOND;
    }
    Timer::Timer(const Timer &timer) : Timer() {
        *this = timer;
    }
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

    void Timer::start() {
        LARGE_INTEGER i;
        QueryPerformanceCounter(&i);
        ct = i.QuadPart;
    }
    long double Timer::get(unsigned type) {
        LARGE_INTEGER i;
        QueryPerformanceCounter(&i);
        long double endTime = static_cast<long double>(i.QuadPart - ct), sFreq = freq;
        for (unsigned i = 0; i < type; ++i) {
            sFreq *= 1000.L;
        }
        return endTime / sFreq;
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
