#ifndef JUTIL_IS_FLOATING_POINT_H
#define JUTIL_IS_FLOATING_POINT_H

#include "Validator.hpp"
#include "BaseOf.hpp"

namespace jutil {

    template <typename D>
    struct FloatingPoint {

        typedef D DerivedType;

        virtual explicit operator long double() const = 0;
    };

    template <typename T>
    struct IsFloatingPoint : Validator<bool, BaseOf<FloatingPoint<T>, T>::Value || IsSame<T, float>::Value || IsSame<T, double>::Value || IsSame<T, long double>::Value> {};

}

#endif // JUTIL_IS_FLOATING_POINT_H
