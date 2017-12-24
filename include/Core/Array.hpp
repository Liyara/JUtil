#ifndef JUTIL_ARRAY_H
#define JUTIL_ARRAY_H

#include "Pointer.hpp"

namespace jutil {
    template <typename T>
    class Array : private NonCopyable {
    public:
        Array(T *p = JUTIL_NULLPTR) : arr(p) {}
        T &operator[](size_t i) {
            return arr[i];
        }
        const T &operator[](size_t i) const {
            return arr[i];
        }
        T *get()  {
            return arr;
        }
        Array<T> &reset(T *p = JUTIL_NULLPTR) {
            destroy();
            arr = p;
            return *this;
        }
        ~Array() {
            destroy();
        }
    private:
        T *arr;
        void destroy() {
            if (arr) {
                delete [] arr;
                arr = JUTIL_NULLPTR;
            }
        }
    };
}

#endif // JUTIL_ARRAY_H
