#include "JUtil/Core/Allocator.h"
#include <cstring>
#include <stdlib.h>

namespace jutil {
    bool Allocator::move(void *d, const void *s, size_t c) {
        return memmove(d, s, c);
    }

    bool Allocator::copy(void *d, const void *s, size_t c) {
        return memcpy(d, s, c);
    }

    Allocator::~Allocator() {}
}
