#ifndef JUTIL_IS_RVAL_H
#define JUTIL_IS_RVAL_H

#include "Validator.hpp"

namespace jutil {
    template <typename T>
    struct IsRVal : FalseType {};

    template <typename T>
    struct IsRVal<T&&> : TrueType {};
}

#endif // JUTIL_IS_RVAL_H
