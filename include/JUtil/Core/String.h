#ifndef JUTIL_STRING_H
#define JUTIL_STRING_H

#include "JUtil/Core/Tuple.hpp"
#include "JUtil/Container/Queue.hpp"

#define STRING_ERR_INDEX   0x0A

#ifdef JUTIL_ERR

    #define STRINGERR JUTIL_ERR_STRING

    #define STRINGERR_OUTER             ((CORE_ERR_INDEX << 4) | STRING_ERR_INDEX)
    #define STRINGERR_INNER_CHARACTER   0x0000
    #define STRINGERR_INNER_CONVERSION  0x0001
    #define STRINGERR_INNER_BASE        0x0002
    #define STRINGERR_CHARACTER_MSG     "Invalid character"
    #define STRINGERR_CONVERSION_MSG    "Conversion failed"
    #define STRINGERR_BASE_MSG          "Attempt to instantiate invalid String type"
    #define STRINGERR_CHARACTER_INVOKE  JUTIL_INVOKE_ERR(STRINGERR_OUTER, STRINGERR_INNER_CHARACTER, JUTIL_ERR_MSG(STRINGERR_CHARACTER_MSG))
    #define STRINGERR_CONVERSION_INVOKE JUTIL_INVOKE_ERR(STRINGERR_OUTER, STRINGERR_INNER_CONVERSION, JUTIL_ERR_MSG(STRINGERR_CONVERSION_MSG))
    #define STRINGERR_BASE_INVOKE       JUTIL_INVOKE_ERR(STRINGERR_OUTER, STRINGERR_INNER_BASE, JUTIL_ERR_MSG(STRINGERR_BASE_MSG))

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

#define __STRING_BUFFER__ ((8 * sizeof(long long) - 1) / 3 + 2)

#define __STRING_BASE_ERR__ {STRINGERR_BASE_INVOKE;}

#define __STRING_CAST_CONSTRUCTORS__(type) \


#define __STRING_CONSTRUCTORS__(type)\
    StringBase() {}\
    StringBase(const StringBase<type> &str) {\
        insert(str);\
    }\
    StringBase(type c) {\
        insert(c);\
    }\
    template <size_t l>\
    StringBase(const type (&c)[l]) {\
        this->reserve(l);\
        for (JUTIL_INIT(const type *i, c); (*i) && (*i != '\0'); ++i) insert(*i);\
    }\
    StringBase(const type *c, size_t l = 0) {\
        this->reserve(l);\
        for (JUTIL_INIT(const type *i, c); (*i) && (*i != '\0'); ++i) insert(*i);\
    }

#define __STRING_DO_CONVERSION__(type, func, ret) type *__arr__ = new type[size() + 1];array(__arr__);ret __val__ = cstr_conversion::strTo##func(__arr__);delete[] __arr__;return __val__

#define __STRING_CONVERSION_OPERATORS__(type)\
    JUTIL_EXPL_ operator signed long long() JUTIL_CN_   {__STRING_DO_CONVERSION__(type, LL, signed long long);}\
    JUTIL_EXPL_ operator unsigned long long() JUTIL_CN_ {__STRING_DO_CONVERSION__(type, ULL, unsigned long long);}\
    JUTIL_EXPL_ operator signed long() JUTIL_CN_        {__STRING_DO_CONVERSION__(type, L, signed long);}\
    JUTIL_EXPL_ operator unsigned long() JUTIL_CN_      {__STRING_DO_CONVERSION__(type, UL, unsigned long);}\
    JUTIL_EXPL_ operator signed int() JUTIL_CN_         {__STRING_DO_CONVERSION__(type, I, signed int);}\
    JUTIL_EXPL_ operator unsigned int() JUTIL_CN_       {__STRING_DO_CONVERSION__(type, UI, unsigned int);}\
    JUTIL_EXPL_ operator signed short() JUTIL_CN_       {__STRING_DO_CONVERSION__(type, S, signed short);}\
    JUTIL_EXPL_ operator unsigned short() JUTIL_CN_     {__STRING_DO_CONVERSION__(type, US, unsigned short);}\
    JUTIL_EXPL_ operator double() JUTIL_CN_             {__STRING_DO_CONVERSION__(type, D, double);}\
    JUTIL_EXPL_ operator long double() JUTIL_CN_        {__STRING_DO_CONVERSION__(type, LD, long double);}\
    JUTIL_EXPL_ operator float() JUTIL_CN_              {__STRING_DO_CONVERSION__(type, F, float);}
#define JUTIL_BYTE_STRING_ (unsigned char*)

namespace jutil JUTIL_PUBLIC_ {

    typedef unsigned char Byte;

    template <typename T>
    class JUTIL_PRIVATE_ StringBase : public Queue<T> {
    public:
        StringBase()                                            __STRING_BASE_ERR__
        StringBase(const Queue<T>&)                             __STRING_BASE_ERR__
        template <typename U> StringBase(const StringBase<U>&)  __STRING_BASE_ERR__
        StringBase(T)                                           __STRING_BASE_ERR__
        template <size_t l> StringBase(const T (&c)[l])         __STRING_BASE_ERR__
        StringBase(const T *c, size_t l = 0)                    __STRING_BASE_ERR__
    };

    int toCString(char*, size_t, const char*, ...);
    int toCString(wchar_t*, size_t, const wchar_t*, ...);
    int toCString(unsigned char*, size_t, const unsigned char*, ...);

    template <typename T>
    StringBase<char> convert_char(T t, const char *p);

    template <typename T>
    StringBase<wchar_t> convert_wchar_t(T t, const wchar_t *p);

    template <typename T>
    StringBase<Byte> convert_Byte(T t, const Byte *p);

    //Wrappers for cstd string conversion functions (e.g. strtoll, atoi, etc.)
    namespace JUTIL_PRIVATE_ cstr_conversion {
        long long strToLL(const char*);
        long long strToLL(const wchar_t*);
        long long strToLL(const unsigned char*);

        unsigned long long strToULL(const char*);
        unsigned long long strToULL(const wchar_t*);
        unsigned long long strToULL(const unsigned char*);

        long strToL(const char*);
        long strToL(const wchar_t*);
        long strToL(const unsigned char*);

        unsigned long strToUL(const char*);
        unsigned long strToUL(const wchar_t*);
        unsigned long strToUL(const unsigned char*);

        int strToI(const char*);
        int strToI(const wchar_t*);
        int strToI(const unsigned char*);

        short strToS(const char*);
        short strToS(const wchar_t*);
        short strToS(const unsigned char*);

        float strToF(const char*);
        float strToF(const wchar_t*);
        float strToF(const unsigned char*);

        double strToD(const char*);
        double strToD(const wchar_t*);
        double strToD(const unsigned char*);

        long double strToLD(const char*);
        long double strToLD(const wchar_t*);
        long double strToLD(const unsigned char*);

        unsigned int strToUI(const char*);
        unsigned int strToUI(const wchar_t*);
        unsigned int strToUI(const unsigned char*);

        unsigned short strToUS(const char*);
        unsigned short strToUS(const wchar_t*);
        unsigned short strToUS(const unsigned char*);
    }

    template <typename T> StringBase<T> substr(const StringBase<T> &th, int s, int e);
    template <typename T> StringBase<T> substr(const StringBase<T> &th, int s);

    template<typename T> StringBase<T> &ltrim(StringBase<T> &str);
    template<typename T> StringBase<T> &rtrim(StringBase<T> &str);
    template<typename T> StringBase<T> &trim(StringBase<T> &str);

    template<typename T> StringBase<T> upperCase(StringBase<T> str);
    template<typename T> StringBase<T> lowerCase(StringBase<T> str);
    template<typename T> StringBase<T> ToggleCase(StringBase<T> str);

    template <typename T> Queue<StringBase<T> > split(const StringBase<T> &str, T delim);
    template <typename T> Queue<StringBase<T> > split(const StringBase<T> &str, const StringBase<T> &delim);

    //Character String

    template<>
    class JUTIL_PUBLIC_ StringBase<char> : public Queue<char> {
    public:

        __STRING_CONSTRUCTORS__(char);

        JUTIL_EXPL_ StringBase(bool num) JUTIL_N_ {*this = convert_char(num, "%u");}
        JUTIL_EXPL_ StringBase(signed long long num) JUTIL_N_ {*this = convert_char(num, "%I64d");}
        JUTIL_EXPL_ StringBase(unsigned long long num) JUTIL_N_ {*this = convert_char(num, "%I64u");}
        JUTIL_EXPL_ StringBase(signed long num) JUTIL_N_ {*this = convert_char(num, "%ld");}
        JUTIL_EXPL_ StringBase(unsigned long num) JUTIL_N_ {*this = convert_char(num, "%lu");}
        JUTIL_EXPL_ StringBase(signed int num) JUTIL_N_ {*this = convert_char(num, "%d");}
        JUTIL_EXPL_ StringBase(unsigned int num) JUTIL_N_ {*this = convert_char(num, "%u");}
        JUTIL_EXPL_ StringBase(signed short num) JUTIL_N_ {*this = convert_char(num, "%hd");}
        JUTIL_EXPL_ StringBase(unsigned short num) JUTIL_N_ {*this = convert_char(num, "%hu");}
        JUTIL_EXPL_ StringBase(double num) JUTIL_N_ {*this = convert_char(num, "%f");}
        JUTIL_EXPL_ StringBase(long double num) JUTIL_N_ {*this = convert_char(num, "%Lf");}
        JUTIL_EXPL_ StringBase(float num) JUTIL_N_ {*this = convert_char(num, "%f");}

        __STRING_CONVERSION_OPERATORS__(char);

        JUTIL_EXPL_ operator bool() JUTIL_CN_ {
            StringBase<char> o(*this);
            jutil::upperCase(jutil::trim(o));
            if (o == StringBase<char>("FALSE") || o == StringBase<char>("0")) return false;
            else return true;
        }

        void array(char arr[]) const {
            size_t i = 0;
            for (Queue<char>::Iterator it = this->begin(); it != this->end(); ++it) {
                arr[i] = *it;
                ++i;
            }
            arr[size()] = 0;
        }

        template <typename U>
        StringBase<char> &operator=(const StringBase<U> &str) {
            clear();
            insert(static_cast<StringBase<char> >(str));
            return *this;
        }
    };

    typedef StringBase<char> String;

    //Wide Character String

    template<>
    class JUTIL_PUBLIC_ StringBase<wchar_t> : public Queue<wchar_t> {
    public:
        __STRING_CONSTRUCTORS__(wchar_t);

        JUTIL_EXPL_ StringBase(bool num) JUTIL_N_ {*this = convert_wchar_t(num, L"%u");}
        JUTIL_EXPL_ StringBase(signed long long num) JUTIL_N_ {*this = convert_wchar_t(num, L"%I64d");}
        JUTIL_EXPL_ StringBase(unsigned long long num) JUTIL_N_ {*this = convert_wchar_t(num, L"%I64u");}
        JUTIL_EXPL_ StringBase(signed long num) JUTIL_N_ {*this = convert_wchar_t(num, L"%ld");}
        JUTIL_EXPL_ StringBase(unsigned long num) JUTIL_N_ {*this = convert_wchar_t(num, L"%lu");}
        JUTIL_EXPL_ StringBase(signed int num) JUTIL_N_ {*this = convert_wchar_t(num, L"%d");}
        JUTIL_EXPL_ StringBase(unsigned int num) JUTIL_N_ {*this = convert_wchar_t(num, L"%u");}
        JUTIL_EXPL_ StringBase(signed short num) JUTIL_N_ {*this = convert_wchar_t(num, L"%hd");}
        JUTIL_EXPL_ StringBase(unsigned short num) JUTIL_N_ {*this = convert_wchar_t(num, L"%hu");}
        JUTIL_EXPL_ StringBase(double num) JUTIL_N_ {*this = convert_wchar_t(num, L"%f");}
        JUTIL_EXPL_ StringBase(long double num) JUTIL_N_ {*this = convert_wchar_t(num, L"%Lf");}
        JUTIL_EXPL_ StringBase(float num) JUTIL_N_ {*this = convert_wchar_t(num, L"%f");}

        __STRING_CONVERSION_OPERATORS__(wchar_t);

        JUTIL_EXPL_ operator bool() JUTIL_CN_ {
            StringBase<wchar_t> o(*this);
            jutil::upperCase(jutil::trim(o));
            if (o == StringBase<wchar_t>(L"FALSE") || o == StringBase<wchar_t>(L"0")) return false;
            else return true;
        }

        void array(wchar_t arr[]) const {
            size_t i = 0;
            for (Queue<wchar_t>::Iterator it = this->begin(); it != this->end(); ++it) {
                arr[i] = *it;
                ++i;
            }
            arr[size()] = 0;
        }

        StringBase<wchar_t> &operator=(const StringBase<wchar_t> &str) {
            clear();
            insert(str);
            return *this;
        }
    };

    typedef StringBase<wchar_t> WideString;

    //Unsigned Character String

    template<>
    class JUTIL_PUBLIC_ StringBase<Byte> : public Queue<Byte> {
    public:
        StringBase() {}
        StringBase(const Queue<Byte> &q) {
            insert(q);
        }
        StringBase(const StringBase<Byte> &str) {
            insert(str);
        }
        StringBase(Byte c) {
            insert(c);
        }

        template <size_t l>
        StringBase(const Byte (&c)[l]) {
            this->reserve(l);
            for (JUTIL_INIT(const Byte *i, c); (unsigned)(i - c) < l; ++i) insert(*i);
        }
        StringBase(const Byte *c, size_t l) {
            this->reserve(l);
            for (JUTIL_INIT(const Byte *i, c); (unsigned)(i - c) < l; ++i) insert(*i);
        }


        #ifdef JUTIL_CPP11
            template <typename X, typename... Y>
            StringBase(X head, Y... tail) : Queue{static_cast<Byte>(head), static_cast<Byte>(tail)...} {}
        #endif

        void array(Byte arr[]) const {
            size_t i = 0;
            for (Queue<Byte>::Iterator it = this->begin(); it != this->end(); ++it) {
                arr[i] = *it;
                ++i;
            }
        }

        StringBase<Byte> &operator=(const StringBase<Byte> &str) {
            clear();
            insert(str);
            return *this;
        }
    };

    typedef StringBase<Byte> ByteString;

    template <typename T>
    StringBase<char> convert_char(T t, const char *p) {
        char cstr[__STRING_BUFFER__];
        toCString(cstr, sizeof(cstr), p, t);
        StringBase<char> str(cstr);
        return str;
    }

    template <typename T>
    StringBase<wchar_t> convert_wchar_t(T t, const wchar_t *p) {
        wchar_t cstr[__STRING_BUFFER__];
        toCString(cstr, sizeof(cstr), p, t);
        StringBase<wchar_t> str(cstr);
        return str;
    }

    template <typename T> StringBase<T> substr(const StringBase<T> &th, int s, int e) {
        StringBase<T> str;
        size_t r = (s > 0 && e > 0? e - s : (s > 0 && e < 0? (th.size() + e) - s : (s < 0 && e > 0? e - (th.size() + s) : (s < 0 && e < 0? (th.size() + e) - (th.size() + s) : 0) ) ) );
        str.reserve(r);
        for (size_t i = (s >= 0? s : th.size() + s); i <= (e >= 0? e : th.size() + e); ++i) {
            str.insert((th)[i]);
        }
        return str;
    }
    template <typename T> StringBase<T> substr(const StringBase<T> &th, int s) {
        return substr(th, s, th.size() - 1);
    }

    template<typename T> StringBase<T> &ltrim(StringBase<T> &str) {
        for (typename StringBase<T>::Iterator i = str.begin(); i != str.end(); ++i) {
            if (*i == ' ' || *i == '\t') str.erase(i);
            else break;
        }
        return str;
    }
    template<typename T> StringBase<T> &rtrim(StringBase<T> &str) {
        for (typename StringBase<T>::Iterator i = str.end() - 1; i; --i) {
            if (*i == ' ' || *i == '\t') str.erase(i);
            else break;
        }
        return str;
    }
    template<typename T> StringBase<T> &trim(StringBase<T> &str) {
        rtrim(ltrim(str));
        return str;
    }

    template<typename T> StringBase<T> upperCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) if (*it >= 97 && *it <= 122) *it -= 32;
        return str;
    }
    template<typename T> StringBase<T> lowerCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) if (*it >= 65 && *it <= 90) *it += 32;
        return str;
    }
    template<typename T> StringBase<T> ToggleCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) {
            if (*it >= 65 && *it <= 90) *it += 32;
            else if (*it >= 97 && *it <= 122) *it -= 32;
        }
        return str;
    }

    template <typename T> Queue<StringBase<T> > split(const StringBase<T> &str, T delim) {
        return split(str, StringBase<T>(delim));
    }
    template <typename T> Queue<StringBase<T> > split(const StringBase<T> &str, const StringBase<T> &delim) {
        size_t start = 0;
        Queue<StringBase<T> > r;
        for (size_t i = 0; i < str.size() - (delim.size() - 1); ++i) {
            if (substr(str, i, i + (delim.size() - 1)) == delim) {
                if (i > start) r.insert(substr(str, start, i - 1));
                start = i + delim.size();
            }
        }
        if (start < str.size()) r.insert(substr(str, start));
        return r;
    }

    String hash(const String&);

    ByteString makeByteString(const Byte[], size_t);
}

template <typename T, typename U>
jutil::StringBase<T> operator+(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    jutil::StringBase<T> n(l);
    n.insert(static_cast<jutil::StringBase<T> >(r));
    return n;
}

template <typename T, typename U>
jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    l.insert(static_cast<jutil::StringBase<T> >(r));
    return l;
}

template <typename T, typename U>
bool operator==(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    if (l.size() == r.size()) {
        for (size_t i = 0; i < l.size(); ++i) if (l[i] != static_cast<T>(r[i])) return false;
        return true;
    } else return false;
}

template <typename T, typename U>
bool operator!=(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    return !(l == r);
}

template <typename T>
jutil::StringBase<T> operator+(const jutil::StringBase<T> &l, const T *r) {
    return l + jutil::StringBase<T>(r);
}

template <typename T>
jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const T *r) {
    return l += jutil::StringBase<T>(r);
}

template <typename T>
jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const T r) {
    l.insert(r);
    return l;
}

template <typename T>
bool operator==(const jutil::StringBase<T> &l, const T *r) {
    return l == jutil::StringBase<T>(r);
}

template <typename T>
bool operator!=(const jutil::StringBase<T> &l, const T *r) {
    return l != jutil::StringBase<T>(r);
}

template <typename T>
jutil::StringBase<T> operator+(const T *r, const jutil::StringBase<T> &l) {
    return l + jutil::StringBase<T>(r);
}

template <typename T>
jutil::StringBase<T> &operator+=(const T *r, jutil::StringBase<T> &l) {
    return l += jutil::StringBase<T>(r);
}

template <typename T>
bool operator==(const T *r, const jutil::StringBase<T> &l) {
    return l == jutil::StringBase<T>(r);
}

template <typename T>
bool operator!=(const T *r, const jutil::StringBase<T> &l) {
    return l != jutil::StringBase<T>(r);
}

template <typename T, typename A> jutil::Queue<T, A>::operator jutil::StringBase<char>() {
    jutil::String str = '{';
    for (size_t i = 0; i < this->size(); ++i) {
        str += jutil::String((*this)[i]);
        str += ",";
    }
    str.erase(str.size() - 1);
    str += "}";
    return str;
}

template <typename T, typename A> jutil::Queue<T, A>::operator const jutil::StringBase<char>() const {
    jutil::String str = '{';
    for (size_t i = 0; i < this->size(); ++i) {
        str += jutil::String((*this)[i]);
        str += ",";
    }
    str.erase(str.size() - 1);
    str += "}";
    return str;
}

#ifdef STRINGERR
    #undef STRINGERR
#endif

#undef __J_ETSC
#undef __J_ESTC
#undef __J_ESC
#undef JUTIL_STRING_DC

#endif // JUTIL_STRING_H
