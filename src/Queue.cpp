#include "Container/Queue.hpp"
#include <cstring>
#include <stdlib.h>

namespace jutil {
    ///Wrappers for C functions.

    void __jutil__queue__move(void *d, void *s, size_t c) {
        memmove(d, s, c);
    }

    void __jutil__queue__copy(void *d, const void *s, size_t c) {
        memcpy(d, s, c);
    }

    void *__jutil__queue__alloc(size_t amount) {
        return malloc(amount);
    }

    void *__jutil__queue__realloc(void *p, size_t amount) {
        return realloc(p, amount);
    }

    bool __jutil__queue_compare(const void *a, const void *b, size_t s) {
        return (!memcmp(a, b, s));
    }

    void __jutil__queue__destroy(void *p) {
        free(p);
    }
}
