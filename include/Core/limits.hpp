#ifndef JUTIL_LIMITS_H
#define JUTIL_LIMITS_H

#include "Traits/IsInteger.hpp"
#include "Traits/IsUnsigned.hpp"
#include "Core/integers.h"

namespace jutil {
    template <typename T>
    inline T upperLimit() {
        size_t len = sizeof(T);
        uint64_t val = 1;
        for (size_t i = 0; i < (len * 8); ++i) {
            val *= 2;
        }
        val -= 1;
        val /= 2;
        if (IsUnsigned<T>::Value) {
            return static_cast<T>(val * 2 + 1);
        } else {
            return (T)val;
        }
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
        if (IsUnsigned<T>::Value) {
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
