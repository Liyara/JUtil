#include "JUtil/Core/MemoryBlock.h"
#include "JUtil/Core/Error.h"
#include "JUtil/Core/String.h"
#include <cstdlib>

namespace jutil JUTIL_PRIVATE_ {
    MemoryBlock::MemoryBlock() JUTIL_N_ : JUTIL_INIT(block, NULL), JUTIL_INIT(count, 0) {
        block = malloc(1);
    }
    MemoryBlock::MemoryBlock(size_t bytes) JUTIL_N_ : JUTIL_INIT(block, NULL), JUTIL_INIT(count, bytes) {
        block = malloc(1);
        alloc(bytes);
    }
    MemoryBlock::~MemoryBlock() JUTIL_N_ {
        //free(block);
        count = 0;
    }
    bool MemoryBlock::alloc(size_t i) {
        count = i;
        if (count > 0) {
            block = realloc(block, i);
        }
        return block;
    }
    size_t MemoryBlock::size() JUTIL_CN_ {
        return count;
    }
    MemoryBlock::MemoryBlock(const MemoryBlock &mb) JUTIL_N_ : JUTIL_INIT(block, mb.block), JUTIL_INIT(count, mb.count) {}
    #ifdef JUTIL_CPP11
        MemoryBlock::MemoryBlock(MemoryBlock &&mb) : JUTIL_INIT(block, mb.block), JUTIL_INIT(count, mb.count) {
            mb.clear();
        }
        MemoryBlock &MemoryBlock::operator=(MemoryBlock &&mb) JUTIL_N_ {
            free(block);
            block = mb.block;
            count = mb.count;
            mb.clear();
            return *this;
        }
    #endif
    MemoryBlock &MemoryBlock::operator=(const MemoryBlock &mb) JUTIL_N_ {
        block = mb.block;
        count = mb.count;
        return *this;
    }
    MemoryBlock &MemoryBlock::clear() JUTIL_N_ {
        JUTIL_NULL_(block);
        count = 0;
        return *this;
    }
    MemoryBlock &MemoryBlock::destroy(size_t i) JUTIL_N_ {
        count -= i;
        alloc(count);
        return *this;
    }
}
