#ifndef JUTIL_DECLVAL_H
#define JUTIL_DECLVAL_H

#include "Traits/TypeManipulators.hpp"

namespace jutil {
    template <typename T>
    struct JUTIL_PRIVATE_ JUTIL_Declval_ final {
        static const bool evaluated = false;
        static typename AddRvalue<T>::Type rType();
    };
    template <typename T>
    typename AddRvalue<T>::Type declval() noexcept {
        static_assert(JUTIL_Declval_<T>::evaluated, "jutil::declval<T>() used in evaluated context!");
        return JUTIL_Declval_<T>::rType();
    }
}

#endif // JUTIL_DECLVAL_H
