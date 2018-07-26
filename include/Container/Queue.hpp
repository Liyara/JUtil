#ifndef JUTIL_QUEUE_H
#define JUTIL_QUEUE_H

#include "ContiguousContainer.hpp"
#include "Core/StringInterface.hpp"

#ifdef JUTIL_CPP11
    #include <initializer_list>
    #include "Traits/Traits.hpp"
#endif

#ifdef JUTIL_ERR

    #define QUEUEERR JUTIL_ERR_QUEUE

    #define QUEUEERR_OUTER               0x0013

#else
    #define QUEUEERR_INDEX_INVOKE
    #define QUEUEERR_ITERATOR_INVOKE
#endif

#include <cstring>
#include <stdlib.h>
#include <iostream>

namespace jutil JUTIL_PUBLIC_ {

    extern "C" {
        void __jutil__queue__move(void*, void*, size_t);
        void __jutil__queue__copy(void*, const void*, size_t);
        void __jutil__queue__destroy(void*);
        void* __jutil__queue__alloc(size_t);
        void* __jutil__queue__realloc(void*, size_t);
        bool __jutil__queue_compare(const void*, const void*, size_t);
    }


    /**

        @class Queue<T>     A memory-consecutive container.
    */
    template <typename T>
    class Queue : public ContiguousContainer<size_t, T, Queue<T> > {
    public:

        /** =========================================================================================================================================

                TYPES

         ** =========================================================================================================================================
        */

        typedef ContiguousContainer<size_t, T, Queue<T> > BaseType;
        typedef typename BaseType::ValueType ValueType;
        typedef typename BaseType::Iterator Iterator;
        typedef Queue<ValueType> Type;

        /** =========================================================================================================================================

                CONSTRUCTORS

         ** =========================================================================================================================================
        */


        /**
            Default constructor. Queue is empty.
        */
        JUTIL_CX_ Queue() JUTIL_N_ : allocated(0), BaseType() {}

        /**
            Copy constructor.

            @param q    The Queue to copy.
        */
        Queue(const Type &q) : Type() {
            reserve(q.size());
            for (unsigned i = 0; i < q.size(); ++i) {
                new (this->block + i) ValueType(q[i]);
            }
            this->count = q.size();
        }

        Queue(const ValueType &v) {
            insert(v);
        }

        Queue(const ValueType &v, size_t num) : Type() {
            reserve(num);
            for (size_t i = 0; i < num; ++i) insert(v);
        }

        /** =========================================================================================================================================

                METHODS

         ** =========================================================================================================================================
        */

        /**
            Reserve memory for future insertions

            @param c    Number of elements to reserve.
            @return     Reference to calling object.
        */
        Queue<T> &reserve(size_t c) {
            if (c > allocated) {
                reallocate(c);
            }
            return *this;
        }

        /**
            Erase an element by index.

            @param n    Index of element to erase.
            @return     Reference to calling object.
        */
        Type &erase(const size_t &n) JUTIL_OVERRIDE_ {
            __jutil__queue__move(this->block + (n), this->block + n + 1, sizeof(ValueType) * (this->count - n));
            --this->count;
            return *this;
        }


        Type &erase(Iterator it) {
            for (size_t i = 0; i < this->count; ++i) {
                if (it == (this->block + i)) {
                    erase(i);
                    break;
                }
            }
            return *this;
        }

        /**
            Insert an element at the end of the Queue.

            @param value    Value of element to be inserted.
            @return         Reference to calling object.
        */
        Type &insert(const ValueType &value) {
            if (allocated <= this->count) {
                reallocate(this->count + 1);
            }
            ++(this->count);
            new (this->block + (this->count - 1)) ValueType(value);
            return *this;
        }

        /**
            Insert an element at the specified index.

            @param value    Value of element to be inserted.
            @param n        Index to insert at.
            @return         Reference to calling object.
        */
        Type &insert(const ValueType &value, const size_t &n) JUTIL_OVERRIDE_ {
            if (n == this->size()) {
                return insert(value);
            } else {
                if (allocated <= this->count) {
                    reallocate(this->count + 1);
                }
                __jutil__queue__move(this->block + n + 1, this->block + n, sizeof(ValueType) * (this->count - n));
                new (this->block + n) ValueType(value);
                ++(this->count);
                return *this;
            }
        }

        /**
            Insert all elements of a Queue at the end of this Queue.

            @param q    Queue to be inserted.
            @return     Reference to calling object.
        */
        Type &insert(const Type &q) {
            reserve(this->count + q.size());
            for (unsigned i = 0; i < q.size(); ++i) {
                new (this->block + i + this->count) ValueType(q[i]);
            }
            this->count += q.size();
            return *this;
        }

        /**
            Insert all elements of a Queue at the specific index.

            @param q    Queue to be inserted.
            @param n    Index to insert at.
            @return     Reference to calling object.
        */
        template <
            typename U
            #ifdef JUTIL_CPP11
            , typename = typename Enable<Convert<U, T>::Value>::Type
            #endif
        >
        Type &insert(const Queue<U> &q, const size_t &n) {
            reserve(this->count + q.size());
            for (Iterator i = q.begin(); i != q.end(); ++i) {
                insert(static_cast<T>(*i), n);
            }
            return *this;
        }

        /**
            Find an element.

            @param value    Value to search for.
            @param p        Pointer to a variable into which the index of the value is filled.
            @return         Wheather or not a matching element was found.
        */
        bool find(const ValueType &value, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            for (JUTIL_INIT(size_t i, 0); i < this->count; ++i) {
                if (__jutil__queue_compare(static_cast<const void*>(&value), static_cast<const void*>(this->block + i), sizeof(ValueType))) {
                    if (p) *p = i;
                    return true;
                }
            }
            return false;
        }

        /**
            Find all instances of an element.

            @param value    Value to search for.
            @return         Queue containing all indices at which a matching value was found.
        */
        Queue<size_t> findAll(const ValueType &value) JUTIL_C_ {
            Queue<T> r;
            for (size_t i = 0; i < this->count; ++i) {
                if (__jutil__queue_compare(static_cast<const void*>(&value), static_cast<const void*>(this->block + i), sizeof(ValueType))) {
                    r.insert((*this)[i]);
                }
            }
            return r;
        }

        /**
            @return     A Queue with the same elements as the calling Queue in reverse order.
        */
        Queue<T> reverse() JUTIL_C_ {
            Queue<T> r;
            r.reserve(this->count);
            for (Iterator i = this->end() - 1; i != this->begin() - 1; --i) {
                r.insert(*i);
            }
            return r;
        }

        /**
            Remove all elements from the Queue, and deallocate all memory from the Queue.

            @return     Reference to calling object.
        */
        Type &clear() {
            this->reset();
            return *this;
        }

        /**
            Changes the number of elements allocated to this Queue. Always prefer @see reserve instead.

            @param amount   Number of elements to allocate.
        */
        void reallocate(size_t amount) {
            if (allocated == 0) {
                this->block = static_cast<ValueType*>(__jutil__queue__alloc(sizeof(ValueType) * amount));
                allocated = amount;
            } else if (amount > allocated) {
                void *nBlock = __jutil__queue__realloc(static_cast<void*>(this->block), sizeof(ValueType) * amount);
                if (nBlock) this->block = static_cast<ValueType*>(nBlock);
                else {
                    ValueType *nAlloc = static_cast<ValueType*>(__jutil__queue__alloc(sizeof(ValueType) * amount));
                    __jutil__queue__copy(nAlloc, this->block, sizeof(ValueType) * allocated);
                    __jutil__queue__destroy(this->block);
                    this->block = nAlloc;
                }
                allocated = amount;
            }
        }

        /**
            @return Reference to the last element in the Queue.
        */
        T &last() {
            return this->block[this->count - 1];
        }

        /**
            @return Reference to the first element in the Queue.
        */
        T &first() {
            return this->block[0];
        }

        /** =========================================================================================================================================

                OPERATORS

         ** =========================================================================================================================================
        */

        /**
            Assigment operator.

            @param q    The Queue to copy.
            @return     Reference to calling object.
        */
        Queue<T> &operator=(const Type &q) {
            free();
            insert(q);
            return *this;
        }

        /**
            Find and element at a specified index.

            @param      Index of element to return.
            @return     Reference to element at the specified index.
        */
        ValueType &operator[](const size_t &n) JUTIL_OVERRIDE_ {
            return this->block[n];
        }

        /**
            Const overload of @see operator[]
        */
        const ValueType &operator[](const size_t &n) JUTIL_CO_ {
            return this->block[n];
        }

        /**
            Comparison operator between two Queues.

            @param other    Queue to compare against.
            @return         Wheather or not the Queues are equivelent.
        */
        const bool operator==(const Type &other) JUTIL_C_ {
            if (this->count == other.count) {
                for (size_t i = 0; i < this->count; ++i) {
                    if ((*this)[i] != other[i]) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
        }

        /**
            Negation of @see operator==
        */
        JUTIL_CX_ bool operator !=(const Type &other) JUTIL_C_ {
            return !((*this) == other);
        }

        /**
            Conversion operator to @see @class String.
        */
        operator jutil::String();

        /**
            Const overload of @see operator jutil::String
        */
        operator const jutil::String() const;

        /** =========================================================================================================================================

                C++11 MOVE SEMANTICS

         ** =========================================================================================================================================
        */
        #ifdef JUTIL_CPP11
            Queue(std::initializer_list<ValueType> &&list) : Type() {
                reserve(list.size());
                for (auto i: list) {
                    insert(i);
                }
            }
            /*Queue(Queue<T> &&q) : allocated(0), BaseType() {
                insert(move(q));
            }*/
            Queue<T> &operator=(Queue<T> &&q) {
                this->clear();
                insert(move(q));
                return *this;
            }
            template <typename U, typename = typename Enable<Convert<U, T>::Value>::Type>
            Type &insert(std::initializer_list<U> &&list) {
                reserve(this->count + list.size());
                for (auto i: list) {
                    insert(static_cast<T>(i));
                }
                return *this;
            }
        #endif

        void free() JUTIL_OVERRIDE_ {
            for (unsigned i = 0; i < this->count; ++i) {
                (this->block + i)->~ValueType();
            }
            __jutil__queue__destroy(this->block);
            this->block = NULL;
            this->allocated = 0;
            this->count = 0;
        }

        ~Queue() {
            free();
        }

    private:

        size_t allocated;
    };
}

#endif // JUTIL_QUEUE_H
