#ifndef JUTIL_STRING_INTERFACE_H
#define JUTIL_STRING_INTERFACE_H

namespace jutil {

    class String;

    struct StringInterface {
        virtual operator String() = 0;
        virtual operator const String() const = 0;
    };
}

#endif // JUTIL_STRING_INTERFACE
