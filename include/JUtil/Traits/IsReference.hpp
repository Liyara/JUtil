#ifndef JUTIL_IS_REFERENCE_H
#define JUTIL_IS_REFERENCE_H

#include "Validator.hpp"

namespace jutil {
    template <typename T>
    struct IsReference : FalseType {};

    template <typename T>
    struct IsReference<T&> : TrueType {};
}

#endif // JUTIL_IS_REFERENCE_H
