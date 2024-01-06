#ifndef JUTIL_SET_H
#define JUTIL_SET_H

#include "JUtil/Container/ContiguousContainer.hpp"

#define SET_ERR_INDEX 0x06

namespace jutil {
    template<
        size_t capacity,
        typename T
    >
    class Set {
    public:

        Set() {
            
        }

        #ifdef JUTIL_CPP11
            template <typename Y, typename... X>
            Set(Y val, X... vals) : data{val, vals...} {}

            Set(Set &&set) {
                (*this) = move(set);
            }

            Set &operator=(Set &&set) {
                memcpy(data, set.data, set.size());
                memset(set.data, 0, set.size());
                return *this;
            }
        #endif

        Set(T val, size_t num) {
            for (size_t i = 0; i < num; ++i) {
                JUTIL_NEW(data + i, T(val));
            }
        }

        Set(const Set &set) {
            for (size_t i = 0; i < set.size(); ++i) {
                JUTIL_NEW(data + i, T(set.data[i]));
            }
        }

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

        virtual size_t size() const {
            return capacity;
        }

        virtual ~Set() {}

        virtual Set<capacity, T> &destroy(size_t i) {
            (data + i)->~T();
            JUTIL_NEW(data + i, T());
            return *this;
        }

        virtual Set<capacity, T> &clear() {
            for (size_t i = 0; i < capacity; ++i) destroy(i);
            return *this;
        }

    protected:
        T data[capacity];

    };

    template<
        size_t capacity,
        typename T,
        typename = typename Enable<IsDefaultConstructible<T>::Value>::Type,
        typename = typename Enable<BaseOf<NonCopyable, T>::Value>::Type
    >
    class NonCopyableSet : public Set<capacity, T, int, int>, public NonCopyable {};
}

#endif // JUTIL_SET_H
