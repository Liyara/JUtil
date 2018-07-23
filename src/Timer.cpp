#include "Core/Timer.h"
#include "Traits/TypeManipulators.hpp"
#include <windows.h>

namespace jutil JUTIL_PUBLIC_ {

    Timer::Timer() : ct(-1), freq(1) {}
    Timer::Timer(const Timer &timer) {
        *this = timer;
    }
    Timer &Timer::operator=(const Timer &timer) {
        ct = timer.ct;
        freq = timer.freq;
        return *this;
    }
    #ifdef JUTIL_CPP11
        Timer::Timer(Timer &&timer) {
            *this = jutil::move(timer);
        }
        Timer &Timer::operator=(Timer &&timer) {
            ct = timer.ct;
            freq = timer.freq;
            timer.ct = -1;
            timer.freq = 1;
            return *this;
        }
    #endif

    void Timer::start() {
        LARGE_INTEGER i;
        QueryPerformanceFrequency(&i);
        freq = static_cast<long double>(i.QuadPart) / 1000000000.L;
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
        freq = 1;
        return r;
    }
}
