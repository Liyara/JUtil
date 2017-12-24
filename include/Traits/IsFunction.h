#ifndef JUTIL_IS_FUNCTION_H
#define JUTIL_IS_FUNCTION_H

#include "Validator.hpp"

namespace jutil {

    template <typename T>
    struct IsFunction : FalseType {};

    template <typename T, typename... Args>
    struct IsFunction<T(Args...)> : TrueType {};

    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...)> : TrueType {};

    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) volatile> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const volatile> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) volatile> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const volatile> : TrueType {};

    template <typename T, typename... Args>
    struct IsFunction<T(Args...)&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...)&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) volatile&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const volatile&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) volatile&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const volatile&> : TrueType {};

    template <typename T, typename... Args>
    struct IsFunction<T(Args...)&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...)&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) volatile&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args...) const volatile&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) volatile&&> : TrueType {};
    template <typename T, typename... Args>
    struct IsFunction<T(Args... ...) const volatile&&> : TrueType {};

}

#endif // JUTIL_IS_FUNCTION_H
