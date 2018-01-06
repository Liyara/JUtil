#ifndef CONTIGUOUS_CONTAINER_H
#define CONTIGUOUS_CONTAINER_H

#include "Container.hpp"

namespace jutil JUTIL_PUBLIC_ {

    template<
        typename K,
        typename T,
        typename D
    >
    class ContiguousContainer : public Container<K, T, D, T*> {
    public:

        typedef K KeyType;
        typedef T ValueType;
        typedef T *Iterator;
        typedef D DerivedType;
        typedef ContiguousContainer<KeyType, ValueType, DerivedType> Type;

        JUTIL_CX_ ContiguousContainer() JUTIL_N_ : block(JUTIL_NULLPTR), count(0) {}

        virtual DerivedType &fillArray(ValueType arr[]) JUTIL_C_ {
            for (size_t i = 0; i < count; ++i) {
                arr[i] = block[i];
            }
            return *(const_cast<DerivedType*>(static_cast<const DerivedType*>(this)));
        }

        virtual const size_t size() JUTIL_CNO_ {
            return count;
        }

        virtual const bool empty() JUTIL_CNO_ {
            return count == 0;
        }

        Iterator begin() JUTIL_NO_ {
            return block;
        }

        Iterator end() JUTIL_NO_ {
            return block + count;
        }

        const Iterator begin() JUTIL_CNO_ {
            return block;
        }

        const Iterator end() JUTIL_CNO_  {
            return block + count;
        }

        virtual ~ContiguousContainer() {
            free();
        }

    protected:
        ValueType *block;
        size_t count;
        void reset() {
            free();
            count = 0;
        }
        void free() {
            if (block) {
                delete [] block;
                block = JUTIL_NULLPTR;
            }
        }
    };
}

#endif // CONTIGUOUS_CONTAINER_H
