#ifndef JUTIL_RNG_H
#define JUTIL_RNG_H

#include "Core/Macro.h"
#include "Traits/IsInteger.hpp"
#include "Core/integers.h"
#include "Core/limits.hpp"
#include "IO/IO.h"

#define _JUTIL_RNG_MAX_INT uint64_t
#define _JUTIL_RNG_MIN_INT int64_t
#define _JUTIL_RNG_MAX_FLT long double
#define _JUTIL_RNG_MIN_FLT long double

namespace jutil JUTIL_PUBLIC_ {
    class JUTIL_PUBLIC_ RNG {
    public:

        RNG();
        RNG(long double, long double);
        RNG(const RNG&);
        RNG &operator=(const RNG&);

        #ifdef JUTIL_CPP11
            RNG(RNG&&);
            RNG &operator=(RNG&&);
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

        void setFloor(long double);

        void setCeiling(long double);

    private:
        long double floor, ceiling;
        void *_time;
        int64_t internalValue;
        void shake();
    };
}

#endif // JUTIL_RNG_H
