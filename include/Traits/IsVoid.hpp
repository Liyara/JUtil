#ifndef JUTIL_IS_VOID_H
#define JUTIL_IS_VOID_H

#include "Validator.hpp"
#include "TypeManipulators.hpp"

namespace jutil {
    template <typename T>
    struct IsVoidBase : FalseType {};

    template <>
    struct IsVoidBase<void> : TrueType {};

    template <typename T>
    struct IsVoid :  IsVoidBase<typename RemoveConst<T>::Type> {};
}

#endif // JUTIL_IS_VOID_H
