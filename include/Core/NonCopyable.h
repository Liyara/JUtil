#ifndef JUTIL_NON_COPYABLE_H
#define JUTIL_NON_COPYABLE_H

#include "version.h"

#define JUTIL_NONCOPYABLE private NonCopyable

namespace jutil {
    struct NonCopyable {
        #ifdef JUTIL_CPP11
        public:
            NonCopyable() = default;
            NonCopyable(const NonCopyable&) = delete;
            NonCopyable(NonCopyable&&) = delete;
            NonCopyable &operator=(const NonCopyable&) = delete;
            NonCopyable &operator=(NonCopyable&&) = delete;
        #else
        private:
            NonCopyable(const NonCopyable&);
            NonCopyable &operator=(const NonCopyable&);
        #endif
    };
}

#endif // JUTIL_NON_COPYABLE_H
