#ifndef JUTIL_IS_UNSIGNED_H
#define JUTIL_IS_UNSIGNED_H

#if __cplusplus >= 201103L

#include "Validator.hpp"
#include "IsArithmatic.hpp"

namespace jutil {

    template <typename T>
    struct IsUnsigned : Validator<bool, IsArithmatic<T>::Value && !(static_cast<T>(-1) < static_cast<T>(0))>::Type {};

}

#endif

#endif // JUTIL_IS_UNSIGNED_H
