#ifndef JUTIL_POINTER_H
#define JUTIL_POINTER_H

#include "NonCopyable.h"
#include "Macro.h"

namespace jutil {

    template <typename T>
    class Pointer : private NonCopyable {
    public:
        JUTIL_CX_ Pointer(T *p = JUTIL_NULLPTR) : raw(p) {}
        JUTIL_CX_ Pointer(T &&t) : raw(new T(t)) {}
        JUTIL_CX_ T *get() JUTIL_C_ {
            return raw;
        }
        Pointer<T> &reset(T *p = JUTIL_NULLPTR) {
            destroy();
            raw = p;
        }
        virtual ~Pointer() {
            destroy();
        }
        T &operator*() {
            return *raw;
        }
        T *operator->() {
            return raw;
        }

    private:
        T *raw;
        void destroy() {
            if (raw) {
               delete raw;
                raw = JUTIL_NULLPTR;
            }
        }
    };
}

#endif // JUTIL_POINTER_H
