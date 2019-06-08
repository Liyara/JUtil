#include "JUtil/Container/Queue.hpp"
#include <cstring>
#include <stdlib.h>

namespace jutil {

    void *__QueueInternalAllocator::alloc(size_t c) {
        return malloc(c);
    }
    void *__QueueInternalAllocator::realloc(void *p, size_t c) {
        return ::realloc(p, c);
    }
    bool __QueueInternalAllocator::free(void *p) {
        if (p) ::free(p);
        return p;
    }
    __QueueInternalAllocator::~__QueueInternalAllocator() {}


}
