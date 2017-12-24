#ifndef JUTIL_IS_ARRAY_H
#define JUTIL_IS_ARRAY_H

#include "Validator.hpp"
#include "Core/size_t.h"

namespace jutil {
    template <typename T>
    struct IsArray : jutil::FalseType {};

    template <typename T>
    struct IsArray<T[]> : jutil::TrueType {};

    template <typename T, size_t size>
    struct IsArray<T[size]> : jutil::TrueType {};
}

#endif // JUTIL_IS_ARRAY_H
