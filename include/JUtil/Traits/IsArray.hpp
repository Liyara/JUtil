#ifndef JUTIL_IS_ARRAY_H
#define JUTIL_IS_ARRAY_H

#include "Validator.hpp"
#include "JUtil/Core/size_t.h"

namespace jutil {
    template <typename T>
    struct IsArray : FalseType {};

    template <typename T>
    struct IsArray<T[]> : TrueType {};

    template <typename T, size_t size>
    struct IsArray<T[size]> : TrueType {};
}

#endif // JUTIL_IS_ARRAY_H
