#ifndef JUTIL_SET_H
#define JUTIL_SET_H

#include "Container/ContiguousContainer.hpp"

#ifdef JUTIL_CPP11
    #include "Traits/Traits.hpp"
#endif

namespace jutil {
    template<size_t capacity, typename T>
    class Set {
    public:

        Set() {}

        #ifdef JUTIL_CPP11
            template <typename Y, typename... X>
            Set(Y val, X... vals) : data{val, vals...} {}

            Set(Set &&set) {
                (*this) = jutil::move(set);
            }

            Set &operator=(Set &&set) {
                data = set.data;
                set.data = NULL;
            }
        #endif

        Set(T val, size_t num) {
            for (size_t i = 0; i < num; ++i) {
                JUTIL_NEW(data + i, T(val));
            }
        }

        Set(const Set &set) : data(set.data) {}

        Set &operator=(const Set &set) {
            for (size_t i = 0; i < capacity; ++i) {
                (data + i)->~T();
                JUTIL_NEW(data + i, T(set.data[i]));
            }
            return *this;
        }

        virtual T &operator[](size_t i) {
            return data[i];
        }

        virtual const T &operator[](size_t i) const {
            return data[i];
        }

        virtual T *begin() {
            return data;
        }

        virtual T *end() {
            return data + capacity;
        }

        virtual const T *begin() const {
            return data;
        }

        virtual const T *end() const {
            return data + capacity;
        }

        size_t size() const {
            return capacity;
        }

        virtual ~Set() {}

        T data[capacity];

    protected:

    };
}

#endif // JUTIL_SET_H
