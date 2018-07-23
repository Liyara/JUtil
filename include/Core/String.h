#ifndef JUTIL_STRING_H
#define JUTIL_STRING_H

#include "Container/Queue.hpp"

#ifdef JUTIL_CPP11
    #include "Traits/Traits.hpp"
#endif

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

#define ENOUGH ((8 * sizeof(long long) - 1) / 3 + 2)


namespace jutil JUTIL_PUBLIC_ {
    class String : public jutil::Queue<char> {
        public:
        template <size_t l>
        String(const char (&c)[l]) {
            this->reserve(l);
            for (JUTIL_INIT(const char *i, c); (*i) && (*i != '\0'); ++i) {
                if (validCharacter(*i)) {
                    insert(*i);
                } else {
                    STRINGERR_CHARACTER_INVOKE;
                    break;
                }
            }
        }

        String(const char *c, size_t l = 0) {
            this->reserve(l);
            for (JUTIL_INIT(const char *i, c); (*i) && (*i != '\0'); ++i) {
                if (validCharacter(*i)) {
                    insert(*i);
                } else {
                    STRINGERR_CHARACTER_INVOKE;
                    break;
                }
            }
        }

        String(const Queue<char>&);
        String(const String&);
        String(char);

        int toCString(char*, size_t, const char*, ...);

        template <typename T>
        String convert(T t, const char *p) {
            char cstr[ENOUGH];
            toCString(cstr, sizeof(cstr), p, t);
            String str(cstr);
            return str;
        }

        JUTIL_EXPL_ operator bool() JUTIL_CN_;
        JUTIL_EXPL_ operator signed long long() JUTIL_CN_;
        JUTIL_EXPL_ operator unsigned long long() JUTIL_CN_;
        JUTIL_EXPL_ operator signed long() JUTIL_CN_;
        JUTIL_EXPL_ operator unsigned long() JUTIL_CN_;
        JUTIL_EXPL_ operator signed int() JUTIL_CN_;
        JUTIL_EXPL_ operator unsigned int() JUTIL_CN_;
        JUTIL_EXPL_ operator signed short() JUTIL_CN_;
        JUTIL_EXPL_ operator unsigned short() JUTIL_CN_;
        JUTIL_EXPL_ operator double() JUTIL_CN_;
        JUTIL_EXPL_ operator long double() JUTIL_CN_;
        JUTIL_EXPL_ operator float() JUTIL_CN_;

        JUTIL_EXPL_ String(bool) JUTIL_N_;
        JUTIL_EXPL_ String(signed long long) JUTIL_N_;
        JUTIL_EXPL_ String(unsigned long long) JUTIL_N_;
        JUTIL_EXPL_ String(signed long) JUTIL_N_;
        JUTIL_EXPL_ String(unsigned long) JUTIL_N_;
        JUTIL_EXPL_ String(signed int) JUTIL_N_;
        JUTIL_EXPL_ String(unsigned int) JUTIL_N_;
        JUTIL_EXPL_ String(signed short) JUTIL_N_;
        JUTIL_EXPL_ String(unsigned short) JUTIL_N_;
        JUTIL_EXPL_ String(double) JUTIL_N_;
        JUTIL_EXPL_ String(long double) JUTIL_N_;
        JUTIL_EXPL_ String(float) JUTIL_N_;

        String &operator=(const String&);

        JUTIL_CX_ String() JUTIL_N_ : jutil::Queue<char>() {}
        String operator+(const String&) JUTIL_CN_;

        String &operator+=(const String&) JUTIL_N_;
        bool operator==(const String&) JUTIL_CN_;

        bool operator!=(const String&) JUTIL_CN_;

        JUTIL_CX_ inline static bool validCharacter(char c) JUTIL_N_ {
            return ((c >= 0x00) && (c <= 0x7f));
        }

        String substr(int, int) JUTIL_CN_;
        String substr(int) JUTIL_CN_;

        String &ltrim();
        String &rtrim();
        String &trim();

        String upperCase() JUTIL_CN_;
        String lowerCase() JUTIL_CN_;
        String toggleCase() JUTIL_CN_;

        virtual ~String() JUTIL_N_;

        /**
            @section array(char[])

            Fills the array passed in with the values in the string.

            The array passed in should be 1 longer than the string's length,
            because array() will automatically null-terminate the array.
        */
        void array(char[]) JUTIL_CN_;

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
    };

    template <typename T>
    const String qAsString(const Queue<T> &q)  {
        String s = "{";
        s.reserve(q.size());
        for (typename Queue<T>::Iterator i = q.begin(); i != q.end(); ++i) {
            s += String::toString(*i) + ((i + 1 == q.end())? "" : ", ");
        }
        s += "}";
        return s;
    }

    template <typename T>
    jutil::Queue<T>::operator const jutil::String() const {
        return qAsString(*this);
    }

    template <typename T>
    jutil::Queue<T>::operator jutil::String() {
        return qAsString(*this);
    }
}

inline jutil::String operator+(const char *c, const jutil::String &s) {
    return jutil::String(c) + s;
}

/*inline jutil::String operator+(const jutil::String &s, const char *c) {
    return s + jutil::String(c);
}

inline jutil::String operator+(const jutil::String &s, char c) {
    return s + jutil::String::toString(c);
}

inline jutil::String operator+(char c, const jutil::String &s) {
    return jutil::String::toString(c) + s;
}*/

#ifdef STRINGERR
    #undef STRINGERR
#endif

#undef __J_ETSC
#undef __J_ESTC
#undef __J_ESC
#undef JUTIL_STRING_DC

#endif // JUTIL_STRING_H
