#ifndef JUTIL_MEMORY_H
#define JUTIL_MEMORY_H

#include "Macro.h"

namespace jutil JUTIL_PUBLIC_ {

    typedef unsigned char uchar;

    class JUTIL_PUBLIC_ MemoryBlock {
    public:
        MemoryBlock() JUTIL_N_;
        MemoryBlock(size_t) JUTIL_N_;
        MemoryBlock(const MemoryBlock&) JUTIL_N_;
        #ifdef JUTIL_CPP11
            MemoryBlock(MemoryBlock&&);
            MemoryBlock &operator=(MemoryBlock&&) JUTIL_N_;
        #endif
        MemoryBlock &operator=(const MemoryBlock&) JUTIL_N_;
        template <typename T>
        const T &get(size_t index) JUTIL_CN_ {
            JUTIL_INIT(uchar *cb, reinterpret_cast<uchar*>(block));
            cb += index;
            JUTIL_INIT(T *tBlock, reinterpret_cast<T*>(cb));
            return *tBlock;
        }
        template <typename T>
        T &get(size_t index) JUTIL_N_ {
            return const_cast<T&>(static_cast<const MemoryBlock&>(*this).get<T>(index));
        }
        template <typename T>
        MemoryBlock &set(size_t index, T t) JUTIL_N_ {
            JUTIL_INIT(T *r, &(get<T>(index)));
            *r = t;
            return *this;
        }

        MemoryBlock &destroy(size_t) JUTIL_N_;
        MemoryBlock &clear() JUTIL_N_;
        bool alloc(size_t);
        size_t size() JUTIL_CN_;
        ~MemoryBlock() JUTIL_N_;
    private:
        void *block;
        size_t count;
    };
}

#endif // JUTIL_MEMORY_H
