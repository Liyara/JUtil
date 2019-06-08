#ifndef JUTIL_IS_NULL_POINTER_H
#define JUTIL_IS_NULL_POINTER_H

#include "Validator.hpp"

namespace jutil {
    template <typename T>
    struct IsNullPointer : FalseType {};

    template <>
    struct IsNullPointer<decltype(nullptr)> : TrueType {};
}

#endif // JUTIL_IS_NULL_POINTER_H
