#include "Core/String.h"
#include <cstdio>
#include "IO/IO.h"

#define ENOUGH ((8 * sizeof(long long) - 1) / 3 + 2)
#define __USE_MINGW_ANSI_STDIO 1

namespace jutil JUTIL_PRIVATE_ {

    String::String(const char *c) : List() {
        for (JUTIL_INIT(const char *i, c); (*i) && (*i != '\0'); ++i) {
            if (validCharacter(*i)) {insert(*i);}
            else {
                STRINGERR_CHARACTER_INVOKE;
                break;
            }
        }
    }
    String::String(const List<char> &lc) {
        jutil::out << "test" << jutil::endl;
        for (JUTIL_INIT(unsigned i, 0); i < lc.size(); ++i) {
            if (validCharacter(lc[i])) {insert(lc[i]);}
            else {
                STRINGERR_CHARACTER_INVOKE;
                break;
            }
        }
    }
    String& String::ltrim() {
        for (String::Iterator i = begin(); i != end(); ++i) {
            if (*i == ' ' || *i == '\t') {
                erase(i);
            } else {
                break;
            }
        }
        return *this;
    }
    String& String::rtrim() {
        for (String::Iterator i = end() - 1; i.get(); --i) {
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
    JUTIL_C_ List<size_t> String::replace(const String &a, const String &b) {
        size_t interval = a.size();
        for (JUTIL_INIT(size_t i, 0); i < size() - interval; ++i) {
            String s = substr(i, i + interval);
        }
    }
    String String::operator+(const String &str) JUTIL_CN_ {
        JUTIL_INIT(String cpy, *this);
        cpy += str;
        return cpy;
    }
    String &String::operator+=(const String &str) JUTIL_N_ {
        if (!(str.empty())) {
            insert(str);
        }
        return *this;
    }
    bool String::operator==(const String &str) JUTIL_CN_ {
        if (size() != str.size()) {
            return false;
        } else {
            for (JUTIL_INIT(size_t i, 0); i < size(); ++i) {
                if ((*this)[i] != str[i]) {
                    return false;
                }
            }
        }
        return true;
    }
    String String::operator+(const char &c) JUTIL_CN_ {
        String str = *this;
        str.insert(c);
        return str;
    }
    String &String::operator+=(const char &c) JUTIL_N_ {
        *this = *this + c;
        return *this;
    }

    String String::substr(int s, int e) JUTIL_CN_ {
        String str;
        for (size_t i = (s >= 0? s : this->size() + s); i <= (e >= 0? e : this->size() + e); ++i) {
            str += (*this)[i];
        }
        return str;
    }

    String String::substr(int s) JUTIL_CN_ {
        return substr(s, length - 1);
    }

    bool String::operator!=(const String &str) JUTIL_CN_ {return !((*this) == (str));}
    JUTIL_CX_ bool String::validCharacter(char c) JUTIL_N_ {return ((c >= 0x00) && (c <= 0x7f));}
    void String::array(char arr[]) JUTIL_CN_ {
        size_t i = 0;
        for (auto &it: *this) {
            arr[i] = it;
            ++i;
        }
        arr[size()] = '\0';
    }
    String::String(bool b) JUTIL_N_ {
        if (b) {
            *this = "True";
        } else {
            *this = "False";
        }
    }
    String::String(signed long long num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%I64d", num);
        String str(cstr);
        *this = str;
    }
    String::String(unsigned long long num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%I64u", num);
        String str(cstr);
        *this = str;
    }
    String::String(signed long num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%ld", num);
        String str(cstr);
        *this = str;
    }
    String::String(unsigned long num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%lu", num);
        String str(cstr);
        *this = str;
    }
    String::String(signed int num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%d", num);
        String str(cstr);
        *this = str;
    }
    String::String(unsigned int num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%u", num);
        String str(cstr);
        *this = str;
    }
    String::String(signed short num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%hd", num);
        String str(cstr);
        *this = str;
    }
    String::String(unsigned short num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%hu", num);
        String str(cstr);
        *this = str;
    }
    String::String(double num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%f", num);
        String str(cstr);
        *this = str;
    }
    String::String(long double num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%Lf", num);
        String str(cstr);
        *this = str;
    }
    String::String(float num) JUTIL_N_ {
        char cstr[ENOUGH];
        snprintf(cstr, sizeof(cstr), "%f", num);
        String str(cstr);
        *this = str;
    }
    String::operator signed long long() JUTIL_CN_ {
        signed long long r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%I64d", &r);
        return r;
    }
    String::operator unsigned long long() JUTIL_CN_ {
        unsigned long long r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%I64u", &r);
        return r;
    }
    String::operator signed long() JUTIL_CN_ {
        signed long r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%ld", &r);
        return r;
    }
    String::operator unsigned long() JUTIL_CN_ {
        unsigned long r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%lu", &r);
        return r;
    }
    String::operator signed int() JUTIL_CN_ {
        signed int r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%d", &r);
        return r;
    }
    String::operator unsigned int() JUTIL_CN_ {
        unsigned int r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%u", &r);
        return r;
    }
    String::operator signed short() JUTIL_CN_ {
        signed short r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%hd", &r);
        return r;
    }
    String::operator unsigned short() JUTIL_CN_ {
        unsigned short r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%hu", &r);
        return r;
    }
    String::operator float() JUTIL_CN_ {
        float r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%f", &r);
        return r;
    }
    String::operator double() JUTIL_CN_ {
        double r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%f", &r);
        return r;
    }
    String::operator long double() JUTIL_CN_ {
        long double r;
        char arr[size() + 1];
        array(arr);
        arr[size()] = '\0';
        sscanf(arr, "%Lf", &r);
        return r;
    }

    String::~String() JUTIL_N_ {clear();}
}



