#include "Core/RNG.h"
#include "Traits/TypeManipulators.hpp"
#include <time.h>

#define JUTIL_RNG_MAX __LDBL_MAX__
#define JUTIL_RNG_MIN __LDBL_MIN__
#define _JUTIL_RNG_INVERTIBLE_MULTIPLIER 0x2545F4914F6CDD1D

namespace jutil {
    RNG::RNG() : RNG(JUTIL_RNG_MIN + 1, JUTIL_RNG_MAX - 1) {}
    RNG::RNG(long double f, long double c) : floor(f), ceiling(c) {
        internalValue = time(0);
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
        internalValue ^= internalValue >> 12;
        internalValue ^= internalValue << 25;
        internalValue ^= internalValue >> 27;
        internalValue *= _JUTIL_RNG_INVERTIBLE_MULTIPLIER;
    }
}
