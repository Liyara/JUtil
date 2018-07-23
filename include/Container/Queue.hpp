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

namespace jutil JUTIL_PUBLIC_ {

    extern "C" {
        void __jutil__queue__move(void*, void*, size_t);
        void __jutil__queue__copy(void*, void*, size_t);
        void* __jutil__queue__alloc(size_t);
        void* __jutil__queue__realloc(void*, size_t);
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
        Queue(const Queue<T> &q) : Type() {
            insert(q);
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
            ValueType *nBlock = static_cast<ValueType*>(::operator new[](sizeof(ValueType) * allocated));
            for (size_t i = 0; i < this->count; ++i) {
                if (i != n) {
                    new (&nBlock[(i < n? i : (i > n? (i - 1) : -1))]) ValueType(this->block[i]);
                }
            }
            --(this->count);
            this->free();
            this->block = nBlock;
            return *this;
        }

        Type &erase(size_t &n, bool) {
            auto &r = erase(n);
            --n;
            return r;
        }

        /**
            Erase an element by iterator.

            @param it       Iterator.
            @param loop     Wheather or not the function is being called from inside a loop.
            @return         If "loop" is true, iterator to new element in the same position as the erased one. Otherwise, "it".
        */
        template <
            typename X
            #ifdef JUTIL_CPP11
            , typename = typename jutil::Enable<jutil::IsSame<T, X>::Value>::Type
            #endif
        >
        X *erase(X *it, bool loop = false) {
            for (size_t i = 0; i < this->count; ++i) {
                if (it == &((*this)[i])) {
                    it = it - (loop? 1 : 0);
                    erase(i);
                }
            }
            return it;
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
            new (&this->block[this->count - 1]) ValueType(value);
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
                    allocated = this->count + 1;
                    ValueType *nBlock = static_cast<ValueType*>(::operator new[](sizeof(ValueType) * allocated));
                    for (size_t i = 0; i < this->count + 1; ++i) {
                        new (&(nBlock[i])) ValueType((i > n? this->block[i - 1] : (i < n? this->block[i] : value)));
                    }
                    this->free();
                    this->block = nBlock;
                } else {
                    __jutil__queue__move(&(this->block[n + 1]), &(this->block[n]), sizeof(ValueType) * (this->count - n));
                    new (&(this->block[n])) ValueType(value);
                }
                ++(this->count);
            }
            return *this;
        }

        /**
            Insert all elements of a Queue at the end of this Queue.

            @param q    Queue to be inserted.
            @return     Reference to calling object.
        */
        template <
            typename U
            #ifdef JUTIL_CPP11
            , typename = typename Enable<Convert<U, T>::Value>::Type
            #endif
        >
        Type &insert(const Queue<U> &q) {
            reserve(this->count + q.size());
            for (Iterator i = q.begin(); i != q.end(); ++i) {
                insert(static_cast<T>(*i));
            }
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
        JUTIL_CX_ bool find(const ValueType &value, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            for (JUTIL_INIT(size_t i, 0); i < this->count; ++i) {
                if ((*this)[i] == value) {
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
        Queue<size_t> findAll(const ValueType &value) {
            Queue<T> r;
            for (size_t i = 0; i < this->count; ++i) {
                if ((*this)[i] == value) {
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
            allocated = 0;
            this->reset();
            return *this;
        }

        /**
            Changes the number of elements allocated to this Queue. Always prefer @see reserve instead.

            @param amount   Number of elements to allocate.
        */
        void reallocate(size_t amount) {
            ValueType *nBlock = static_cast<ValueType*>(::operator new[](sizeof(ValueType) * amount));
            size_t l = 0;
            if (this->count < amount) {
                l = this->count;
            } else {
                l = amount;
            }
            this->count = l;
            for (size_t i = 0; i < this->count; ++i) {
                new (&nBlock[i]) ValueType(this->block[i]);
            }
            this->free();
            this->block = nBlock;
            allocated = amount;
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
        Queue<T> &operator=(const Queue<T> &q) {
            allocated = 0;
            this->reset();
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
            Queue(Queue<T> &&q) : allocated(0), BaseType() {
                insert(move(q));
            }
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

        virtual ~Queue() {}

    private:

        size_t allocated;
    };
}

#endif // JUTIL_QUEUE_H
