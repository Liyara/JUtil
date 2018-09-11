#ifndef JUTIL_VALIDATOR_H
#define JUTIL_VALIDATOR_H

#if __cplusplus >= 201103L

namespace jutil {
    template <bool b, typename T>
    struct JUTIL_PUBLIC_ Enable {};

    template <typename T>
    struct JUTIL_PUBLIC_ Enable<true, T> {typedef T Type;};

    typedef char (&Valid)[1];
    typedef char (&Invalid)[2];

    template<class T, T v>
    struct Validator {
        enum {Value = v};
        typedef T ValueType;
        typedef Validator Type;
        constexpr operator ValueType() const noexcept { return Value; }
    };

    typedef Validator<bool, true> TrueType;
    typedef Validator<bool, false> FalseType;

    template <typename T, typename Y>
    struct IsSame : FalseType {};

    template <typename T>
    struct IsSame<T, T> : TrueType {};

}

#endif

#endif // JUTIL_VALIDATOR_H
