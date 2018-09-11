#ifndef JUTIL_LIMITS_H
#define JUTIL_LIMITS_H

#include "Traits/IsInteger.hpp"
#include "Traits/IsUnsigned.hpp"
#include "Core/integers.h"

#ifndef __FLT_MAX__
    #define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F
#endif // __FLT_MAX__

#ifndef __DBL_MAX__
    #define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)
#endif // __DBL_MAX__

#ifndef __LDBL_MAX__
    #define __LDBL_MAX__ 1.18973149535723176502126385303097021e+4932L
#endif // __LDBL_MAX__

namespace jutil {
    template <typename T>
    inline T upperLimit() {
        uint64_t val = 1;
        for (size_t i = 0; i < (sizeof(T) * 8); ++i) val <<= 1;
        val = (val - 1) >> 1;
        if (static_cast<T>(-1) > static_cast<T>(0)) {
            val = (val << 1) + 1;
        }
        return *reinterpret_cast<T*>(&val);
    }

    template <>
    inline float upperLimit<float>() {
        return __FLT_MAX__;
    }

    template <>
    inline double upperLimit<double>() {
        return __DBL_MAX__;
    }

    template <>
    inline long double upperLimit<long double>() {
        return __LDBL_MAX__;
    }

    template <typename T>
    inline T lowerLimit() {
        if (static_cast<T>(-1) > static_cast<T>(0)) {
            return 0;
        } else {
            return -upperLimit<T>() - 1;
        }
    }

    template <>
    inline float lowerLimit<float>() {
        return -__FLT_MAX__ - 1;
    }

    template <>
    inline double lowerLimit<double>() {
        return -__DBL_MAX__ - 1;
    }

    template <>
    inline long double lowerLimit<long double>() {
        return -__LDBL_MAX__ - 1;
    }
}

#endif // JUTIL_LIMITS_H
