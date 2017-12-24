#ifndef JUTIL_STRING_H
#define JUTIL_STRING_H

#include "Container/List.hpp"

#ifdef JUTIL_CPP11
    #include "Traits/Traits.hpp"
#endif

/**
    @section DESCRIPTION

    @class jutil::String is a a wrapper for C char arrays.

    It is implemented by inheriting a char List. @see List.hpp

    jutil::String can be be 2-way cast with every primitive type.
*/

#ifdef JUTIL_ERR

    #define STRINGERR JUTIL_ERR_STRING

    #define STRINGERR_OUTER             0x0001
    #define STRINGERR_INNER_CHARACTER   0x0000
    #define STRINGERR_INNER_CONVERSION  0x0001
    #define STRINGERR_CHARACTER_MSG     "Invalid character"
    #define STRINGERR_CONVERSION_MSG    "Conversion failed"
    #define STRINGERR_CHARACTER_INVOKE  JUTIL_INVOKE_ERR(STRINGERR_OUTER, STRINGERR_INNER_CHARACTER, JUTIL_ERR_MSG(STRINGERR_CHARACTER_MSG))
    #define STRINGERR_CONVERSION_INVOKE JUTIL_INVOKE_ERR(STRINGERR_OUTER, STRINGERR_INNER_CONVERSION, JUTIL_ERR_MSG(STRINGERR_CONVERSION_MSG))

#else
    #define STRINGERR_CHARACTER_INVOKE
    #define STRINGERR_CONVERSION_INVOKE
#endif

#if defined(JUTIL_TRAITS_H) && defined(JUTIL_CPP11)
    #define JUTIL_TRAITS_ENABLED
#endif

#ifdef JUTIL_TRAITS_ENABLED
    #define JUTIL_ENFORCE_TYPE_STRING_CONVERSION(T) = typename jutil::Enable<jutil::Convert<T, jutil::String>::Value, T>::Type
    #define JUTIL_ENFORCE_STRING_TYPE_CONVERSION(T) = typename jutil::Enable<jutil::Convert<jutil::String, T>::Value, T>::Type
    #define JUTIL_ENFORCE_STRING_CONVERSION_(T)     = JUTIL_ENFORCE_TYPE_STRING_CONVERSION(T)
    #define JUTIL_ENFORCE_STRING_CONVERSION(T)      JUTIL_ENFORCE_TYPE_STRING_CONVERSION(T),\
                                                    typename JUTIL_ENFORCE_STRING_TYPE_CONVERSION(T)

    #define __J_ETSC    , typename JUTIL_ENFORCE_TYPE_STRING_CONVERSION(T)
    #define __J_ESTC    , typename JUTIL_ENFORCE_STRING_TYPE_CONVERSION(T)
    #define __J_ESC     , typename JUTIL_ENFORCE_STRING_CONVERSION(T)

#else
    #define __J_ETSC
    #define __J_ESTC
    #define __J_ESC
#endif

#define JUTIL_STRING_DC(a) JUTIL_EXPL_ String(signed a) JUTIL_N_;\
    JUTIL_EXPL_ String(unsigned a) JUTIL_N_;\
    JUTIL_EXPL_ operator signed a() JUTIL_CN_;\
    JUTIL_EXPL_ operator unsigned a() JUTIL_CN_;

namespace jutil JUTIL_PUBLIC_ {

    class JUTIL_PUBLIC_ String : public List<char> {
    public:
        String(const char*);
        JUTIL_CALL_UNROLL(
            JUTIL_STRING_DC,
            long long,
            long,
            int,
            short
        )
        JUTIL_EXPL_ String(double) JUTIL_N_;
        JUTIL_EXPL_ String(float) JUTIL_N_;
        JUTIL_EXPL_ String(long double) JUTIL_N_;
        JUTIL_EXPL_ String(bool) JUTIL_N_;
        JUTIL_CX_ String() JUTIL_N_ : List() {}
        String operator+(const String&) JUTIL_CN_;
        String operator+(const char&) JUTIL_CN_;
        String &operator+=(const String&) JUTIL_N_;
        String operator+=(const char&) JUTIL_N_;
        bool operator==(const String&) JUTIL_CN_;
        bool operator!=(const String&) JUTIL_CN_;
        JUTIL_EXPL_ operator float() JUTIL_CN_;
        JUTIL_EXPL_ operator double() JUTIL_CN_;
        JUTIL_EXPL_ operator long double() JUTIL_CN_;

        JUTIL_CX_ static bool validCharacter(char) JUTIL_N_;

        const String substr(int, int) JUTIL_CN_;

        virtual ~String() JUTIL_N_;

        /**
            @section array(char[])

            Fills the array passed in with the values in the string.

            The array passed in should be 1 longer than the string's length,
            because array() will automatically null-terminate the array.
        */
        void array(char[]) JUTIL_CN_ JUTIL_OVERRIDE_;

        /**
            @section TEMPLATED CONVERTERS

            toString(T t)               returns the static string conversion of [t].
                                        Includes const overload.
            fromStringTo(const char *s) return the static cats from String([s]) to T.

            When using C++11, These methods will throw a jutil::Error
            if an invalid conversion is attempted.

            When using pre-C++11, the compiler will disallow
            invalid conversions.
        */

        template <typename T __J_ETSC>
        static const String toString(const T &t) {
            return static_cast<String>(t);
        }

        #ifdef JUTIL_CPP11
            template <typename T __J_ETSC>
            static const String toString(T &&t) {
                return static_cast<const String>(t);
            }
        #endif

        template <typename T __J_ESTC>
        static T fromStringTo(const char *s) {
            return static_cast<T>(String(s));
        }

        template <typename T __J_ESTC>
        static T fromStringTo(const String &s) {
            return static_cast<T>(s);
        }

        #ifdef JUTIL_TRAITS_ENABLED
            template <typename... T>
            static const String toString(const T&... t) {
                STRINGERR_CONVERSION_INVOKE;
                return String();
            }
            template <typename... T>
            static const void fromStringTo(const T&... t) {
                STRINGERR_CONVERSION_INVOKE;
            }
        #endif

        /*#ifdef JUTIL_TRAITS_ENABLED
            static String toString(...) {
                STRINGERR_CONVERSION_INVOKE;
                return String();
            }
            template <typename T>
            static T fromStringTo(...) {
                STRINGERR_CONVERSION_INVOKE;
                return {};
            }
        #endif*/
    };

    struct StringInterface {
        virtual operator String() = 0;
        virtual operator const String() const = 0;
    };

}

template <typename T>
jutil::List<T>::operator const jutil::String() const {
    String s = "{";
    for (Iterator i = this->begin(); i != this->end(); ++i) {
        s += String::toString(i) + String(", ");
    }
    s = s.substr(0, -3);
    s += "}";
    return s;
}

inline jutil::String operator+(const char *c, const jutil::String &s) {
    return jutil::String(c) + s;
}

inline jutil::String operator+(const jutil::String &s, const char *c) {
    jutil::String cstr(c);
    return s + jutil::String(c);
}

#ifdef STRINGERR
    #undef STRINGERR
#endif

#undef __J_ETSC
#undef __J_ESTC
#undef __J_ESC
#undef JUTIL_STRING_DC

#endif // JUTIL_STRING_H
