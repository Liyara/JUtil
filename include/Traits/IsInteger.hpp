#ifndef JUTIL_IS_INTEGER_H
#define JUTIL_IS_INTEGER_H

#if __cplusplus >= 201103L

#include "Validator.hpp"
#include "BaseOf.hpp"
#include "IsFloatingPoint.hpp"
#include "IsArithmatic.hpp"

namespace jutil {

    template <typename D>
    struct Integer {

        typedef D DerivedType;

        virtual explicit operator long long() const = 0;
        virtual DerivedType fromInt(long long) const = 0;
    };

    template <typename T>
    struct IsInteger : Validator<bool, BaseOf<Integer<T>, T>::Value || (IsArithmatic<T>::Value && !(IsFloatingPoint<T>::Value))> {};

}

#endif

#endif // JUTIL_IS_INTEGER_H
