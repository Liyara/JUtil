#ifndef JUTIL_RNG_H
#define JUTIL_RNG_H

#include "JUtil/Traits/IsInteger.hpp"
#include "JUtil/Core/limits.hpp"
#include "JUtil/IO/IO.h"
#include "JUtil/Traits/TypeManipulators.hpp"
#include <time.h>
#ifdef JUTIL_WINDOWS
    #include <windows.h>
#endif

#define JUTIL_RNG_MAX __DBL_MAX__
#define JUTIL_RNG_MIN (-__DBL_MAX__ - 1)

#define RNG_ERR_INDEX   0x0F

#define _JUTIL_RNG_MAX_INT uint64_t
#define _JUTIL_RNG_MIN_INT int64_t
#define _JUTIL_RNG_MAX_FLT long double
#define _JUTIL_RNG_MIN_FLT long double

namespace jutil JUTIL_PUBLIC_ {

    inline int64_t queryEntropicSources(void** time) {
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

    class JUTIL_PUBLIC_ RNG {
    public:

        RNG() 
        #ifdef JUTIL_CPP11
            : RNG(JUTIL_RNG_MIN + 1, JUTIL_RNG_MAX - 1) {}
        #else
            : floor(JUTIL_RNG_MIN + 1), ceiling(JUTIL_RNG_MAX - 1) {
                internalValue = queryEntropicSources(&_time);
            }
        #endif
        RNG(long double f, long double c) : floor(f), ceiling(c) {
            internalValue = queryEntropicSources(&_time);
        }
        RNG(const RNG &o) {
            (*this) = o;
        }
        RNG &operator=(const RNG &o) {
            floor = o.floor;
            ceiling = o.ceiling;
            internalValue = o.internalValue;
            return *this;
        }

        #ifdef JUTIL_CPP11
        RNG(RNG &&o) {
            (*this) = jutil::move(o);
        }
        RNG &operator=(RNG &&o) {
            floor = o.floor;
            ceiling = o.ceiling;
            internalValue = o.internalValue;
            o.floor = 0;
            o.ceiling = 0;
            o.internalValue = 0;
            return *this;
        }
    #endif

        #ifdef JUTIL_CPP11
            template <typename T>
            void get(T *v, typename jutil::Enable<IsInteger<T>::Value>::Type* = 0) {
                shake();
                _JUTIL_RNG_MAX_INT up = upperLimit<T>();
                _JUTIL_RNG_MIN_INT down = lowerLimit<T>();
                T
                    min = (down > floor? down : floor),
                    max = (up < (ceiling + 1)? up : ceiling + 1) - min,
                    r = static_cast<T>(internalValue),
                    r2 = min + ((r % max + max) % max)
                ;
                *v = r2;
            }

            template <typename T>
            void get(T *v, typename jutil::Enable<IsFloatingPoint<T>::Value>::Type* = 0) {
                shake();
                _JUTIL_RNG_MIN_INT up = upperLimit<T>();
                _JUTIL_RNG_MIN_INT down = lowerLimit<T>();
                T
                    min = (down > floor? down : floor),
                    max = this->ceiling,
                    norm = static_cast<T>(internalValue) / static_cast<T>(up),
                    diff = max - min,
                    scaled = min + (norm * (norm < 0? -diff : diff));
                ;
                *v = scaled;
            }
        #else
            template <typename T>
            void get(T *v) {
                shake();
                _JUTIL_RNG_MAX_INT up = upperLimit<T>();
                _JUTIL_RNG_MIN_INT down = lowerLimit<T>();
                T
                    min = (down > floor? down : floor),
                    max = this->ceiling,
                    norm = static_cast<T>(internalValue) / static_cast<T>(up),
                    diff = max - min,
                    scaled = min + (norm * (norm < 0? -diff : diff));
                ;
                *v = scaled;
            }
        #endif

        template <typename T>
        T get() {
            T f;
            get<T>(&f);
            return f;
        }

        void setFloor(long double f) {
            floor = f;
        }

        void setCeiling(long double c) {
            ceiling = c;
        }

   

    private:
        long double floor, ceiling;
        void *_time;
        int64_t internalValue;
         void shake() {
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
    };
    
}

#endif // JUTIL_RNG_H
