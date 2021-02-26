#ifndef JUTIL_ALLOCATOR_H
#define JUTIL_ALLOCATOR_H

#include "JUtil/Core/Macro.h"
#include <cstring>
#include <stdlib.h>

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

    bool Allocator::move(void *d, const void *s, size_t c) {
        return memmove(d, s, c);
    }

    bool Allocator::copy(void *d, const void *s, size_t c) {
        return memcpy(d, s, c);
    }

    Allocator::~Allocator() {}
}

#endif // JUTIL_ALLOCATOR_H
