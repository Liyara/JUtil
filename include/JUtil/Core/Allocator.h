#ifndef JUTIL_ALLOCATOR_H
#define JUTIL_ALLOCATOR_H

#include "JUtil/Core/Macro.h"

#define ALLOCATOR_ERR_INDEX   0x08

namespace jutil JUTIL_PUBLIC_ {
    class JUTIL_PUBLIC_ Allocator {
    protected:

        virtual void *alloc(size_t) = 0;
        virtual void *realloc(void*, size_t) = 0;
        virtual bool free(void*) = 0;

        virtual bool move(void*, const void*, size_t);
        virtual bool copy(void*, const void*, size_t);

        virtual ~Allocator();

    };
}

#endif // JUTIL_ALLOCATOR_H
