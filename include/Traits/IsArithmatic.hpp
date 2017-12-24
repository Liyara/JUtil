#ifndef JUTILIS_ARITHMATIC_H
#define JUTILIS_ARITHMATIC_H

#include "Validator.hpp"
#include "Core/Array.hpp"
#include "Convert.hpp"
#include "declval.hpp"

#define ADD_T(a, b) decltype(jutil::declval<a>() + jutil::declval<b>())
#define SUBTRACT_T(a, b) decltype(jutil::declval<a>() - jutil::declval<b>())
#define MULTIPLY_T(a, b) decltype(jutil::declval<a>() * jutil::declval<b>())
#define DIVIDE_T(a, b) decltype(jutil::declval<a>() / jutil::declval<b>())

namespace jutil {

    template<typename...> struct voidify {using Type = void;};

    template <typename T, typename = void>
    struct IsArithmatic : FalseType {};

    template <typename T>
    struct IsArithmatic<
        T,
        typename voidify<
            ADD_T(T, T),
            SUBTRACT_T(T, T),
            MULTIPLY_T(T, T),
            DIVIDE_T(T, T),
            ADD_T(T, long double),
            SUBTRACT_T(T, long double),
            MULTIPLY_T(T, long double),
            DIVIDE_T(T, long double)
        >::Type
    > : TrueType {};
}

#endif // JUTILIS_ARITHMATIC_H
