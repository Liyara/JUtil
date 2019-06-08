#include "JUtil/Core/String.h"
#include <JUtil/Core/integers.h>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cwchar>
#include "JUtil/IO/IO.h"

/*#define ENOUGH ((8 * sizeof(long long) - 1) / 3 + 2)
#define __USE_MINGW_ANSI_STDIO 1
#define CONV char cstr[size() + 1]; array(cstr)
#define CONV_(func) CONV; char *end; return func(cstr, &end, 10)
#define CONV__(func) CONV; return func(cstr)
#define FCONV_(func) CONV; char *end; return func(cstr, &end)*/

namespace jutil JUTIL_PRIVATE_ {

    namespace cstr_conversion {
        long long strToLL(const char *c) {
            return strtoll(c, NULL, 10);
        }
        long long strToLL(const wchar_t *w) {
            return wcstoll(w, NULL, 10);
        }

        unsigned long long strToULL(const char *c) {
            return strtoull(c, NULL, 10);
        }
        unsigned long long strToULL(const wchar_t *w) {
            return wcstoull(w, NULL, 10);
        }

        long strToL(const char *c) {
            return strtol(c, NULL, 10);
        }
        long strToL(const wchar_t *w) {
            return wcstol(w, NULL, 10);
        }

        unsigned long strToUL(const char *c) {
            return strtoul(c, NULL, 10);
        }
        unsigned long strToUL(const wchar_t *w) {
            return wcstoul(w, NULL, 10);
        }

        int strToI(const char *c) {
            return atoi(c);
        }
        int strToI(const wchar_t *w) {
            return (int)strToL(w);
        }

        unsigned int strToUI(const char *c) {
            return atoi(c);
        }
        unsigned int strToUI(const wchar_t *w) {
            return (unsigned int)strToUL(w);
        }

        short strToS(const char *c) {
            return atoi(c);
        }
        short strToS(const wchar_t *w) {
            return (short)strToL(w);
        }

        unsigned short strToUS(const char *c) {
            return atoi(c);
        }
        unsigned short strToUS(const wchar_t *w) {
            return (unsigned short)strToL(w);
        }

        float strToF(const char *c) {
            return strtof(c, NULL);
        }
        float strToF(const wchar_t *w) {
            return wcstof(w, NULL);
        }

        double strToD(const char *c) {
            return strtod(c, NULL);
        }
        double strToD(const wchar_t *w) {
            return wcstod(w, NULL);
        }

        long double strToLD(const char *c) {
            return strtold(c, NULL);
        }
        long double strToLD(const wchar_t *w) {
            return wcstold(w, NULL);
        }
    }

    int toCString(char *c, size_t s, const char *m, ...) {
        va_list list;
        va_start(list, m);
        return vsnprintf(c, s, m, list);
    }

    int toCString(wchar_t *c, size_t s, const wchar_t *m, ...) {
        va_list list;
        va_start(list, m);
        #ifdef JUTIL_MINGW
            return vswprintf(c, m, list);
        #else
            return vswprintf(c, s, m, list);
        #endif
    }

    #if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
      || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
    #define get16bits(d) (*((const uint16_t *) (d)))
    #endif

    #if !defined (get16bits)
    #define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                           +(uint32_t)(((const uint8_t *)(d))[0]) )
    #endif

    String hash(const String &str) {
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

    ByteString makeByteString(const Byte arr[], size_t len) {
        ByteString r;
        for (size_t i = 0; i < len; ++i) {
            r.insert(arr[i]);
        }
        return r;
    }
}

/*

    String::String(const Queue<char> &q) : Queue() {
        insert(q);
    }



     String::String(bool b) JUTIL_N_ {
        if (b) {
            *this = "True";
        } else {
            *this = "False";
        }
     }

    String::operator bool() JUTIL_CN_ {
        jutil::String o = *this;
        o.trim();
        if (o.upperCase() == "FALSE" or o == "1") {
            return false;
        } else {
            return true;
        }
    }
    String::operator signed long long() JUTIL_CN_ {
        CONV_(strtoll);
    }
    String::operator unsigned long long() JUTIL_CN_ {
        CONV_(strtoull);
    }
    String::operator signed long() JUTIL_CN_ {
        CONV_(strtol);
    }
    String::operator unsigned long() JUTIL_CN_ {
        CONV_(strtoul);
    }
    String::operator signed int() JUTIL_CN_{
        CONV__(atoi);
    }
    String::operator unsigned int() JUTIL_CN_ {
        CONV__(atoi);
    }
    String::operator signed short() JUTIL_CN_ {
        CONV__(atoi);
    }
    String::operator unsigned short() JUTIL_CN_ {
        CONV__(atoi);
    }
    String::operator double() JUTIL_CN_ {
        FCONV_(strtod);
    }
    String::operator long double() JUTIL_CN_ {
        FCONV_(strtold);
    }
    String::operator float() JUTIL_CN_ {
        FCONV_(strtof);
    }

     String::String(signed long long num) JUTIL_N_ : Queue() {
        *this = convert(num, "%I64d");
    }
     String::String(unsigned long long num) JUTIL_N_ : Queue() {
        *this = convert(num, "%I64u");
    }
     String::String(signed long num) JUTIL_N_ : Queue() {
        *this = convert(num, "%ld");
    }
     String::String(unsigned long num) JUTIL_N_ : Queue() {
        *this = convert(num, "%lu");
    }
     String::String(signed int num) JUTIL_N_ : Queue() {
        *this = convert(num, "%d");
    }
     String::String(unsigned int num) JUTIL_N_ : Queue() {
        *this = convert(num, "%u");
    }
     String::String(signed short num) JUTIL_N_ : Queue() {
        *this = convert(num, "%hd");
    }
     String::String(unsigned short num) JUTIL_N_ : Queue() {
        *this = convert(num, "%hu");
    }
     String::String(double num) JUTIL_N_ : Queue() {
        *this = convert(num, "%f");
    }
     String::String(long double num) JUTIL_N_ : Queue() {
        *this = convert(num, "%Lf");
    }
     String::String(float num) JUTIL_N_ : Queue() {
        *this = convert(num, "%f");
    }

    String &String::operator=(const String &s) {
        clear();
        insert(s);
        return *this;
    }

    String String::operator+(const String &s) JUTIL_CN_ {
        String n;
        n.insert(*this);
        n.insert(s);
        return n;
    }

    String &String::operator+=(const String &s) JUTIL_N_ {
        insert(s);
        return *this;
    }
    bool String::operator==(const String &s) JUTIL_CN_ {
        if (size() == s.size()) {
            for (size_t i = 0; i < size(); ++i) {
                if ((*this)[i] != s[i]) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }

    bool String::operator!=(const String &s) JUTIL_CN_ {
        return !(*this == s);
    }

    String String::substr(int s, int e) JUTIL_CN_ {
        String str;
        size_t r = (s > 0 && e > 0? e - s : (s > 0 && e < 0? (this->size() + e) - s : (s < 0 && e > 0? e - (this->size() + s) : (s < 0 && e < 0? (this->size() + e) - (this->size() + s) : 0) ) ) );
        str.reserve(r);
        for (size_t i = (s >= 0? s : this->size() + s); i <= (e >= 0? e : this->size() + e); ++i) {
            str.insert((*this)[i]);
        }
        return str;
    }
    String String::substr(int s) JUTIL_CN_ {
        return substr(s, size() - 1);
    }

    String &String::ltrim() {
        for (String::Iterator i = begin(); i != end(); ++i) {
            if (*i == ' ' || *i == '\t') {
                erase(i);
            } else {
                break;
            }
        }
        return *this;
    }
    String &String::rtrim() {
        for (String::Iterator i = end() - 1; i; --i) {
            if (*i == ' ' || *i == '\t') {
                erase(i);
            } else {
                break;
            }
        }
        return *this;
    }
    String &String::trim() {
        return rtrim().ltrim();
    }

    String String::upperCase() JUTIL_CN_ {
        String str = *this;
        for (auto &i: str) {
            if (i >= 97 && i <= 122) {
                i -= 32;
            }
        }
        return str;
    }
    String String::lowerCase() JUTIL_CN_ {
        String str = *this;
        for (auto &i: str) {
            if (i >= 65 && i <= 90) {
                i += 32;
            }
        }
        return str;
    }
    String String::toggleCase() JUTIL_CN_ {
        String str = *this;
        for (auto &i: str) {
            if (i >= 65 && i <= 90) {
                i += 32;
            } else if (i >= 97 && i <= 122) {
                i -= 32;
            }
        }
        return str;
    }

    String::~String() JUTIL_N_ {clear();}

    Queue<String> String::split(char delim) JUTIL_C_ {
        return split(String(delim));
    }

    Queue<String> String::split(const String &delim) JUTIL_C_ {
        size_t start = 0;
        Queue<String> r;
        for (size_t i = 0; i < this->size() - (delim.size() - 1); ++i) {
            if (substr(i, i + (delim.size() - 1)) == delim) {
                if (i > start) r.insert(substr(start, i - 1));
                start = i + delim.size();
            }
        }
        if (start < this->size()) r.insert(substr(start));
        return r;
    }

    void String::array(wchar_t arr[]) JUTIL_CN_ {
        size_t i = 0;
        for (auto &it: *this) {
            arr[i] = it;
            ++i;
        }
        arr[size()] = 0;
    }

    void String::array(char arr[]) JUTIL_CN_ {
        size_t i = 0;
        for (auto &it: *this) {
            arr[i] = it;
            ++i;
        }
        arr[size()] = 0;
    }

    String::String(const String &s) : Queue() {
        *this = s;
    }
    String::String(char c) : Queue() {
        if (validCharacter(c)) {
            insert(c);
        } else {
            STRINGERR_CHARACTER_INVOKE;
        }
    }

    String::String(wchar_t w) : Queue() {
        if (validCharacter(w)) {
            insert(w);
        } else {
            STRINGERR_CHARACTER_INVOKE;
        }
    }

}*/



