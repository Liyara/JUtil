#ifndef JUTIL_STRING_INTERFACE_H
#define JUTIL_STRING_INTERFACE_H

#define STRING_INTERFACE_ERR_INDEX   0x09

namespace jutil {

    template <typename>
    class JUTIL_PRIVATE_ StringBase;

    struct StringInterface {
        virtual operator StringBase<char>() = 0;
        virtual operator const StringBase<char>() const = 0;
    };
}

#endif // JUTIL_STRING_INTERFACE
