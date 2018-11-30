#ifndef LIFO_H
#define LIFO_H

#include "Core/Macro.h"

namespace jutil {
    template <typename T>
    class LIFO {
    public:

        typedef T ValueType;

        virtual bool push(const ValueType&) = 0;
        virtual bool pop() = 0;
        virtual ValueType &top() = 0;
        virtual bool empty() JUTIL_C_ = 0;
        virtual size_t capacity() JUTIL_C_ = 0;
        virtual size_t size() JUTIL_C_ = 0;

    };
}

#endif // LIFO_H
