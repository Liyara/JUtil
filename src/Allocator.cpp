#include "Core/Allocator.h"
#include <cstring>
#include <stdlib.h>

namespace jutil {
    bool Allocator::move(void *d, const void *s, size_t c) {
        return !memmove_s(d, c, s, c);
    }

    bool Allocator::copy(void *d, const void *s, size_t c) {
        return !memcpy_s(d, c, s, c);
    }
}
