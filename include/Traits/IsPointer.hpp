#ifndef JUTIL_IS_POINTER_H

#include "Validator.hpp"
#include "TypeManipulators.hpp"

namespace jutil {
    template <typename T>
    struct IsPointerBase : FalseType {};

    template <typename T>
    struct IsPointerBase<T*> : TrueType {};

    template <typename T>
    struct IsPointer :  IsPointerBase<typename RemoveConst<T>::Type> {};
}

#endif // JUTIL_IS_POINTER_H
