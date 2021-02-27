#ifndef JUTIL_STRING_H
#define JUTIL_STRING_H

#include "JUtil/Core/Tuple.hpp"
#include "JUtil/Container/Queue.hpp"
#include "JUtil/Core/integers.h"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cwchar>

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

#ifndef CONCAT
    #define CONCAT + (jutil::String)
#endif

namespace jutil JUTIL_PUBLIC_ {

    typedef unsigned char Byte;

    template <typename T>
    class JUTIL_PRIVATE_ StringBase : public Queue<T> {
    public:
        StringBase() {}
        StringBase(const Queue<T>&) {}
        template <typename U> StringBase(const StringBase<U>&) {}
        StringBase(T) {}
        template <size_t l> StringBase(const T (&c)[l]) {}
        StringBase(const T *c, size_t l = 0) {}
    };

    template <typename T>
    StringBase<char> convert_char(T, const char*);

    template <typename T>
    inline StringBase<wchar_t> convert_wchar_t(T, const wchar_t*);

    namespace JUTIL_PRIVATE_ cstr_conversion {
        inline long long strToLL(const char *c) {
            return strtoll(c, NULL, 10);
        }
        inline long long strToLL(const wchar_t *w) {
            return wcstoll(w, NULL, 10);
        }

        inline unsigned long long strToULL(const char *c) {
            return strtoull(c, NULL, 10);
        }
        inline unsigned long long strToULL(const wchar_t *w) {
            return wcstoull(w, NULL, 10);
        }

        inline long strToL(const char *c) {
            return strtol(c, NULL, 10);
        }
        inline long strToL(const wchar_t *w) {
            return wcstol(w, NULL, 10);
        }

        inline unsigned long strToUL(const char *c) {
            return strtoul(c, NULL, 10);
        }
        inline unsigned long strToUL(const wchar_t *w) {
            return wcstoul(w, NULL, 10);
        }

        inline int strToI(const char *c) {
            return atoi(c);
        }
        inline int strToI(const wchar_t *w) {
            return (int)strToL(w);
        }

        inline unsigned int strToUI(const char *c) {
            return atoi(c);
        }
        inline unsigned int strToUI(const wchar_t *w) {
            return (unsigned int)strToUL(w);
        }

        inline short strToS(const char *c) {
            return atoi(c);
        }
        inline short strToS(const wchar_t *w) {
            return (short)strToL(w);
        }

        inline unsigned short strToUS(const char *c) {
            return atoi(c);
        }
        inline unsigned short strToUS(const wchar_t *w) {
            return (unsigned short)strToL(w);
        }

        inline float strToF(const char *c) {
            return strtof(c, NULL);
        }
        inline float strToF(const wchar_t *w) {
            return wcstof(w, NULL);
        }

        inline double strToD(const char *c) {
            return strtod(c, NULL);
        }
        inline double strToD(const wchar_t *w) {
            return wcstod(w, NULL);
        }

        inline long double strToLD(const char *c) {
            return strtold(c, NULL);
        }
        inline long double strToLD(const wchar_t *w) {
            return wcstold(w, NULL);
        }
    }

    inline int toCString(char *c, size_t s, const char *m, ...) {
        va_list list;
        va_start(list, m);
        return vsnprintf(c, s, m, list);
    }

    inline int toCString(wchar_t *c, size_t s, const wchar_t *m, ...) {
        va_list list;
        va_start(list, m);
        #ifdef JUTIL_MINGW
            return vswprintf(c, m, list);
        #else
            return vswprintf(c, s, m, list);
        #endif
    }

    template <typename T> 
    inline StringBase<T> substr(const StringBase<T> &th, int s, int e) {
        StringBase<T> str;
        size_t r = (s > 0 && e > 0? e - s : (s > 0 && e < 0? (th.size() + e) - s : (s < 0 && e > 0? e - (th.size() + s) : (s < 0 && e < 0? (th.size() + e) - (th.size() + s) : 0) ) ) );
        str.reserve(r);
        for (size_t i = (s >= 0? s : th.size() + s); i <= (e >= 0? e : th.size() + e); ++i) {
            str.insert((th)[i]);
        }
        return str;
    }
    template <typename T> 
    inline StringBase<T> substr(const StringBase<T> &th, int s) {
        return substr(th, s, th.size() - 1);
    }

    template<typename T> 
    inline StringBase<T> &ltrim(StringBase<T> &str) {
        for (typename StringBase<T>::Iterator i = str.begin(); i != str.end(); ++i) {
            if (*i == ' ' || *i == '\t') str.erase(i);
            else break;
        }
        return str;
    }
    template<typename T> 
    inline StringBase<T> &rtrim(StringBase<T> &str) {
        for (typename StringBase<T>::Iterator i = str.end() - 1; i; --i) {
            if (*i == ' ' || *i == '\t') str.erase(i);
            else break;
        }
        return str;
    }
    template<typename T> 
    inline StringBase<T> &trim(StringBase<T> &str) {
        rtrim(ltrim(str));
        return str;
    }

    template<typename T> 
    inline StringBase<T> upperCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) if (*it >= 97 && *it <= 122) *it -= 32;
        return str;
    }
    template<typename T> 
    inline StringBase<T> lowerCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) if (*it >= 65 && *it <= 90) *it += 32;
        return str;
    }
    template<typename T> 
    inline StringBase<T> ToggleCase(StringBase<T> str) {
        for (typename Queue<T>::Iterator it = str.begin(); it != str.end(); ++it) {
            if (*it >= 65 && *it <= 90) *it += 32;
            else if (*it >= 97 && *it <= 122) *it -= 32;
        }
        return str;
    }

    template <typename T> 
    inline Queue<StringBase<T> > split(const StringBase<T> &str, T delim) {
        return split(str, StringBase<T>(delim));
    }
    template <typename T> 
    inline Queue<StringBase<T> > split(const StringBase<T> &str, const StringBase<T> &delim) {
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

    template <typename T>
    inline StringBase<char> convert_char(T t, const char *p) {
        char cstr[__STRING_BUFFER__];
        toCString(cstr, sizeof(cstr), p, t);
        StringBase<char> str(cstr);
        return str;
    }

    template <typename T>
    inline StringBase<wchar_t> convert_wchar_t(T t, const wchar_t *p) {
        wchar_t cstr[__STRING_BUFFER__];
        toCString(cstr, sizeof(cstr), p, t);
        StringBase<wchar_t> str(cstr);
        return str;
    }

    

    

    typedef StringBase<Byte> ByteString;

    template <typename T>
    inline bool regexMatch(const StringBase<T> &str, const StringBase<T> &regex) {
      return false;
    }

    #if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
      || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
    #define get16bits(d) (*((const uint16_t *) (d)))
    #endif

    #if !defined (get16bits)
    #define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                           +(uint32_t)(((const uint8_t *)(d))[0]) )
    #endif

    inline String hash(const String &str) {
        char *cstr = new char[str.size() + 1];
        str.array(cstr);

        char *data = cstr;

        int len = str.size();

        uint32_t hashr = len, tmp;
        int rem;

        if (len <= 0 || data == NULL) return String();

        rem = len & 3;
        len >>= 2;

        /* Main loop */
        for (;len > 0; len--) {
            hashr  += get16bits (data);
            tmp    = (get16bits (data+2) << 11) ^ hashr;
            hashr   = (hashr << 16) ^ tmp;
            data  += 2*sizeof (uint16_t);
            hashr  += hashr >> 11;
        }

        /* Handle end cases */
        switch (rem) {
            case 3: hashr += get16bits (data);
                    hashr ^= hashr << 16;
                    hashr ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                    hashr += hashr >> 11;
                    break;
            case 2: hashr += get16bits (data);
                    hashr ^= hashr << 11;
                    hashr += hashr >> 17;
                    break;
            case 1: hashr += (signed char)*data;
                    hashr ^= hashr << 10;
                    hashr += hashr >> 1;
        }

        /* Force "avalanching" of final 127 bits */
        hashr ^= hashr << 3;
        hashr += hashr >> 5;
        hashr ^= hashr << 4;
        hashr += hashr >> 17;
        hashr ^= hashr << 25;
        hashr += hashr >> 6;

		delete[] cstr;

        return String(hashr);
    }

    inline ByteString makeByteString(const Byte arr[], size_t len) {
        ByteString r;
        for (size_t i = 0; i < len; ++i) {
            r.insert(arr[i]);
        }
        return r;
    }
}

template <typename T, typename U>
inline jutil::StringBase<T> operator+(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    jutil::StringBase<T> n(l);
    n.insert(static_cast<jutil::StringBase<T> >(r));
    return n;
}

template <typename T, typename U>
inline jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    l.insert(static_cast<jutil::StringBase<T> >(r));
    return l;
}

template <typename T, typename U>
inline bool operator==(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    if (l.size() == r.size()) {
        for (size_t i = 0; i < l.size(); ++i) if (l[i] != static_cast<T>(r[i])) return false;
        return true;
    } else return false;
}

template <typename T, typename U>
inline bool operator!=(const jutil::StringBase<T> &l, const jutil::StringBase<U> &r) {
    return !(l == r);
}

template <typename T>
inline jutil::StringBase<T> operator+(const jutil::StringBase<T> &l, const T *r) {
    return l + jutil::StringBase<T>(r);
}

template <typename T>
inline jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const T *r) {
    return l += jutil::StringBase<T>(r);
}

template <typename T>
inline jutil::StringBase<T> &operator+=(jutil::StringBase<T> &l, const T r) {
    l.insert(r);
    return l;
}

template <typename T>
inline bool operator==(const jutil::StringBase<T> &l, const T *r) {
    return l == jutil::StringBase<T>(r);
}

template <typename T>
inline bool operator!=(const jutil::StringBase<T> &l, const T *r) {
    return l != jutil::StringBase<T>(r);
}

template <typename T>
inline jutil::StringBase<T> operator+(const T *l, const jutil::StringBase<T> &r) {
    return jutil::StringBase<T>(l) + r;
}

template <typename T>
inline bool operator==(const T *l, const jutil::StringBase<T> &r) {
    return r == jutil::StringBase<T>(l);
}

template <typename T>
inline bool operator!=(const T *l, const jutil::StringBase<T> &r) {
    return r != jutil::StringBase<T>(l);
}

template <typename T, typename A> 
inline jutil::Queue<T, A>::operator jutil::StringBase<char>() {
    jutil::String str = '{';
    for (size_t i = 0; i < this->size(); ++i) {
        str += jutil::String((*this)[i]);
        str += ",";
    }
    if (str.size() > 1) str.erase(str.size() - 1);
    str += "}";
    return str;
}

template <typename T, typename A> 
inline jutil::Queue<T, A>::operator const jutil::StringBase<char>() const {
    jutil::String str = '{';
    for (size_t i = 0; i < this->size(); ++i) {
        str += jutil::String((*this)[i]);
        str += ",";
    }
    if (str.size() > 1) str.erase(str.size() - 1);
    str += "}";
    return str;
}

inline jutil::StringBase<char> operator "" _str(const char*, size_t);

inline jutil::StringBase<char> operator "" _str(const char *cstr, size_t) {
    return jutil::StringBase<char>(cstr);
}

inline jutil::StringBase<wchar_t> operator "" _wstr(const wchar_t *cstr, size_t) {
    return jutil::StringBase<wchar_t>(cstr);
}

#ifdef STRINGERR
    #undef STRINGERR
#endif

#undef __J_ETSC
#undef __J_ESTC
#undef __J_ESC
#undef JUTIL_STRING_DC

#endif // JUTIL_STRING_H
