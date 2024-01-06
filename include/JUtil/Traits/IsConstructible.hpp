#ifndef JUTIL_IS_CONSTRUCTIBLE_H
#define JUTIL_IS_CONSTRUCTIBLE_H

#include "Validator.hpp"

namespace jutil {

    template <typename T, typename... Args>
    class IsConstructible {
        template <typename U, typename ...UArgs>
        static Valid test(U(*)[sizeof(U(UArgs...))]);

        template <typename U, typename ...UArgs>
        static Invalid test(...);

    public:
        static const bool Value = sizeof(test<T, Args...>(nullptr)) == sizeof(Valid);
    };

    template <typename T>
    class IsDefaultConstructible : public IsConstructible<T> {};

    template <typename T>
    class IsCopyConstructible : public IsConstructible<T, const T&> {};

    template <typename T>
    class IsMoveConstructible : public IsConstructible<T, T&&> {};
}

#endif // JUTIL_IS_CONSTRUCTIBLE_H
