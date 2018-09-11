#ifndef JUTIL_BASE_OF_H
#define JUTIL_BASE_OF_H

#if __cplusplus >= 201103L

#include "Validator.hpp"

namespace jutil {
    template <typename B, typename D>
    class JUTIL_PUBLIC_ BaseOf final {

        template <typename B2, typename D2>
        struct JUTIL_PRIVATE_ Converter final {
            operator B2*() const;
            operator D2*();
        };

        template <typename T>
        static Valid test(D*, T);

        static Invalid test(B*, int);

    public:

        enum {Value = (sizeof(test(Converter<B,D>(), int())) == sizeof(Valid))};

    };
}

#endif

#endif // JUTIL_BASE_OF_H
