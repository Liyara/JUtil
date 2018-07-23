#ifndef JUTIL_RNG_H
#define JUTIL_RNG_H

#include "Core/Macro.h"
#include "Traits/IsInteger.hpp"
#include "Core/integers.h"
#include "Core/limits.hpp"
#include "IO/IO.h"

namespace jutil JUTIL_PUBLIC_ {
    class JUTIL_PUBLIC_ RNG {
    public:

        RNG();
        RNG(long double, long double);
        RNG(const RNG&);
        RNG &operator=(const RNG&);
        RNG(RNG&&);
        RNG &operator=(RNG&&);

        template <typename T>
        void get(T *v, typename jutil::Enable<IsInteger<T>::Value>::Type* = 0) {
            shake();
            int64_t up = upperLimit<T>(), down = lowerLimit<T>();
            T
                min = (down > floor? down : floor),
                max = (up < (ceiling + 1)? up : ceiling + 1) - min,
                r = static_cast<T>(internalValue),
                r2 = min + ((r % max + max) % max)
            ;
            *v = r2;
        }

        template <typename T>
        T get() {
            T f;
            get<T>(&f);
            return f;
        }

        template <typename T>
        void get(T *v, typename jutil::Enable<IsFloatingPoint<T>::Value>::Type* = 0) {
            shake();
            int64_t up = upperLimit<T>(), down = lowerLimit<T>();
            T
                min = (down > floor? down : floor),
                max = this->ceiling,
                norm = static_cast<T>(internalValue) / static_cast<T>(up),
                diff = max - min,
                scaled = min + (norm * (norm < 0? -diff : diff));
            ;
            *v = scaled;
        }

        void setFloor(long double);

        void setCeiling(long double);

    private:
        long double floor, ceiling;
        int64_t internalValue;
        void shake();
    };
}

#endif // JUTIL_RNG_H
