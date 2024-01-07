#ifndef JUTIL_QUEUE_H
#define JUTIL_QUEUE_H

#include "ContiguousContainer.hpp"
#include "JUtil/Core/StringInterface.hpp"
#include <cstring>
#include <cstdlib>

#ifdef JUTIL_CPP11
    #define JUTIL_NULL_PROTECTOR(ot, nt) template <typename nt, typename = typename Enable<IsSame<nt, ot>::Value>::Type>
#else
    #define JUTIL_NULL_PROTECTOR(ot, nt) template <typename nt>
#endif

namespace jutil  {

    enum {
        DESCENDING,
        ASCENDING,
    };

    enum IndexingStrategy {
        BOUNDED,
        CYCLICAL
    };

    class  __QueueInternalAllocator : public Allocator {
       protected:
        void *alloc(size_t c) {
            return malloc(c);
        }
        void *realloc(void *p, size_t c) {
            return ::realloc(p, c);
        }
        bool free(void *p) {
            if (p) ::free(p);
            return p;
        }
        ~__QueueInternalAllocator() {}
    };

    template <
        typename,
        typename = __QueueInternalAllocator>
    class Queue;

    namespace queue_sort {

        template <typename T>
        void __queueSortSwap(T **a, T **b) {
            T temp = **a;
            **a = **b;
            **b = temp;
        }

        template <typename T>
        T *pickPivot(T **array, size_t len, unsigned char mode) {
            if (len > 2) {
                T *middle = *array + (len / 2), *last = *array + (len - 1);
                if (**array > *middle) __queueSortSwap(array, &middle);
                if (**array > *last) __queueSortSwap(array, &last);
                if (*middle > *last) __queueSortSwap(&middle, &last);
                return middle;
            } else
                return *array;
        }

        template <typename T>
        void __queueSort(T **array, T **pivot, T **leftS, T **rightS, unsigned char mode) {
            __queueSortSwap(pivot, rightS);
            *pivot = *rightS;
            --*rightS;

            T *left = *leftS, *right = *rightS;

            while (true) {
                if (mode) {
                    while ((*left < **pivot || *left == **pivot) && left < *rightS + 1) ++left;
                    while ((*right > **pivot || *right == **pivot) && right > *leftS) --right;
                } else {
                    while ((*left > **pivot || *left == **pivot) && left < *rightS + 1) ++left;
                    while ((*right < **pivot || *right == **pivot) && right > *leftS) --right;
                }

                if (left < right) {
                    __queueSortSwap(&left, &right);
                    left = *leftS;
                    right = *rightS;
                } else
                    break;
            }

            ++*rightS;
            __queueSortSwap(&left, pivot);
            *pivot = left;
        }

        template <typename T>
        void _queueSort(T *array, size_t len, unsigned char mode) {
            if (len > 1) {
                T *pivot = pickPivot(&array, len, mode), *left = array, *right = array + (len - 1);
                __queueSort(&array, &pivot, &left, &right, mode);
                size_t splitter = size_t(pivot - array);
                if (pivot != array) _queueSort(array, splitter, mode);
                if (pivot != right) _queueSort(array + splitter + 1, len - splitter - 1, mode);
            }
        }

        template <typename T>
        void __queueInternalSortAscending(Queue<T> *q) {
            _queueSort(&(q->first()), q->size(), ASCENDING);
        }

        template <typename T>
        void __queueInternalSortDescending(Queue<T> *q) {
            _queueSort(&(q->first()), q->size(), DESCENDING);
        }
    }  // namespace queue_sort

    /**

        @class Queue<T>     A memory-consecutive container.
    */
    template <typename T, typename A>
    class Queue : public ContiguousContainer<size_t, T, Queue<T, A> >, private A {
       public:
        /** =========================================================================================================================================

                TYPES

         ** =========================================================================================================================================
        */

        typedef ContiguousContainer<size_t, T, Queue<T, A> > BaseType;
        typedef typename BaseType::ValueType ValueType;
        typedef typename BaseType::Iterator Iterator;
        typedef A Allocator;
        typedef Queue<ValueType, Allocator> Type;

        /** =========================================================================================================================================

                CONSTRUCTORS

         ** =========================================================================================================================================
        */

        /**
            Default constructor. Queue is empty.
        */

        JUTIL__ Queue() JUTIL_N_ : BaseType(), is(BOUNDED), allocated(0) {
            reserve(2);
        }

#ifdef JUTIL_CPP11
        template <typename X, typename... Y>
        Queue(const X &head, const Y &...tail) : Type() {
            reserve(sizeof...(tail) + 1);
            insert(static_cast<T>(head));
            using et = int[];
            et{0, (insert(static_cast<T>(tail)), 0)...};
        }
#endif

        /**
            Copy constructor.

            @param q    The Queue to copy.
        */

        Queue(const Type &q) : Type() {
            insert(q);
        }

        Queue(const ValueType &v, size_t num) : Type() {
            reserve(num);
            for (size_t i = 0; i < num; ++i) insert(v);
        }

        Queue(ValueType *arr, size_t arrSize) {
            allocated = arrSize;
            this->count = arrSize;
            this->block = arr;
        }

        Queue(const ValueType *arr, size_t arrSize) : Type() {
            reserve(arrSize);
            for (size_t i = 0; i < arrSize; ++i) insert(arr[i]);
        }

        template <size_t len>
        Queue(ValueType arr[len])
#ifdef JUTIL_CPP11
            : Queue(arr, len) {
        }
#else
        {
            allocated = len;
            this->count = len;
            this->block = arr;
        }
#endif

        template <size_t len>
        Queue(const ValueType (&arr)[len])
#ifdef JUTIL_CPP11
            : Queue(arr, len) {
        }
#else
        {
            for (size_t i = 0; i < len; ++i) insert(arr[i]);
        }
#endif

        /** =========================================================================================================================================

                METHODS

         ** =========================================================================================================================================
        */

        /**
            Reserve memory for future insertions

            @param c    Number of elements to reserve.
            @return     Reference to calling object.
        */
        Type &reserve(size_t c) {
            if (c > allocated) reallocate(c);
            return *this;
        }

        /**
            Erase an element by index.

            @param n    Index of element to erase.
            @return     Reference to calling object.
        */
        Type &erase(const size_t &n) JUTIL_OVERRIDE_ {
            if (n >= this->count) {
                if (is == CYCLICAL) erase(this->block + (n % this->count));
            } else erase(this->block + n);
            return *this;
        }

        Type &remove(const ValueType &v) {
            size_t pos;
            while (find(v, &pos)) {
                erase(pos);
            }
            return *this;
        }

        Type &trim() {
            reallocate(this->count);
            return *this;
        }

        JUTIL_NULL_PROTECTOR(T, X)
        Type &erase(X *it) {
            if (it > this->block + this->count) {
                if (is == CYCLICAL) it = this->block + ((it - this->block) % this->count);
                else return *this;
            }
            it->~ValueType();
            if (it + 1 != this->end()) this->move(it, it + 1, sizeof(ValueType) * (this->end() - (it + 1)));
            --(this->count);
            return *this;
        }

        /**
            Insert an element at the end of the Queue.

            @param value    Value of element to be inserted.
            @return         Reference to calling object.
        */
        Type &insert(const ValueType &value) {
            ValueType temp(value);
            ++(this->count);
            //reallocation over-estimates space needs in favor of speed.
            if (allocated < this->count) reallocate(this->count << 1);
            #ifdef JUTIL_CPP11
                JUTIL_NEW(this->block + (this->count - 1), ValueType(temp));
            #else
                JUTIL_NEW(this->block + (this->count - 1), temp);
            #endif
            return *this;
        }

        /**
            Insert an element at the specified index.

            @param value    Value of element to be inserted.
            @param n        Index to insert at.
            @return         Reference to calling object.
        */
        Type &insert(const ValueType &value, const size_t &n) JUTIL_OVERRIDE_ {
            if (n > this->count) {
                if (is == CYCLICAL) return insert(value, n % this->count);
            } else {
                if (n == this->count)
                    return insert(value);
                else {
                    ++(this->count);
                    //Copy into buffer to avoid pointer move issues
                    ValueType vC(value);
                    if (allocated < this->count) reallocate(this->count << 1);
                    this->move(this->block + n + 1, this->block + n, sizeof(ValueType) * (this->count - n));
                    JUTIL_NEW(this->block + n, ValueType(vC));
                }
            }
            return *this;
        }

        /**
            Insert all elements of a Queue at the end of this Queue.

            @param q    Queue to be inserted.
            @return     Reference to calling object.
        */
        Type &insert(const Type &q) {
            reserve(this->count + q.count);
            for (size_t i = 0; i < q.count; ++i) JUTIL_NEW(this->block + i + this->count, ValueType(*(q.block + i)));
            this->count += q.count;
            return *this;
        }

        /**
            Insert all elements of a Queue at the specific index.

            @param q    Queue to be inserted.
            @param n    Index to insert at.
            @return     Reference to calling object.
        */
        Type &insert(const Type &q, size_t n) {
            if (is == CYCLICAL) n = n % this->count;
            reserve(this->count + q.size());
            for (Iterator i = q.end() - 1; i != q.begin() - 1; --i) insert(*i, n);
            return *this;
        }

        /**
            Find an element.

            @param value    Value to search for.
            @param p        Pointer to a variable into which the index of the value is filled.
            @return         Wheather or not a matching element was found.
        */
        bool find(const ValueType &value, size_t start, size_t end, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            } else if (start >= this->count || end > this->count) return false;
            if (this->count == 0) return false;

            for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                if (value == *(this->block + i)) {
                    if (p) *p = i;
                    return true;
                }
            }
            return false;
        }

        bool find(const ValueType &value, size_t start, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return find(value, start, this->count, p);
        }

        bool find(const ValueType &value, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            return find(value, 0, this->count, p);
        }

        bool find(const Type &q, size_t start, size_t end, size_t start2, size_t end2, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }

            if (this->count == 0)
                return false;

            if (start >= this->count || end > this->count) {
            } else {
                size_t qIndex;
                bool confirm;
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    qIndex = 0;
                    confirm = true;
                    for (JUTIL_INIT(size_t ii, start2); ii < end2; ++ii) {
                        if (i + qIndex >= this->count || *(this->block + i + qIndex) != q[ii]) {
                            confirm = false;
                            break;
                        }
                        ++qIndex;
                    }
                    if (confirm) {
                        if (p) *p = i;
                        return true;
                    }
                }
            }
            return false;
        }

        bool find(const Type &q, size_t start, size_t end, size_t start2, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }
            return find(q, start, end, start2, q.count, p);
        }

        bool find(const Type &q, size_t start, size_t end, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }
            return find(q, start, end, 0, q.count, p);
        }

        bool find(const Type &q, size_t start, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return find(q, start, this->count, 0, q.count, p);
        }

        bool find(const Type &q, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            return find(q, 0, this->count, 0, q.count, p);
        }

        bool replace(const ValueType &value, const ValueType &value2, size_t start, size_t end) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }
            bool found = false;
            if (this->count == 0)
                return false;
            else {
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    if (value == *(this->block + i)) {
                        new (this->block + i) ValueType(value2);
                        found = true;
                    }
                }
            }
            return found;
        }

        bool replaceFirst(const ValueType &value, const ValueType &value2, size_t start, size_t end) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }
            if (this->count == 0) return false;
            if (start >= this->count || end > this->count) {
            } else {
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    if (value == *(this->block + i)) {
                        new (this->block + i) ValueType(value2);
                        return true;
                    }
                }
            }
            return false;
        }

        bool replace(const ValueType &value, const ValueType &value2, size_t start) {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return replace(value, value2, start, this->count);
        }

        bool replaceFirst(const ValueType &value, const ValueType &value2, size_t start) {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return replaceFirst(value, value2, start, this->count);
        }

        bool replace(const ValueType &value, const ValueType &value2) {
            return replace(value, value2, 0, this->count);
        }

        bool replaceFirst(const ValueType &value, const ValueType &value2) {
            return replaceFirst(value, value2, 0, this->count);
        }

        bool replace(const Type &value, const Type &value2, size_t start, size_t end, size_t start2, size_t end2) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
                start2 = start2 % this->count;
                end2 = end2 % this->count + 1;
            }
            if (this->count == 0) return false;
            if (start >= this->count || end > this->count) {
            } else {
                size_t qIndex;
                bool confirm;
                Queue<size_t, Allocator> indices;
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    qIndex = 0;
                    confirm = true;
                    for (JUTIL_INIT(size_t ii, start2); ii < end2; ++ii) {
                        if (i + qIndex >= this->count || *(this->block + i + qIndex) != value[ii]) {
                            confirm = false;
                            break;
                        }
                        ++qIndex;
                    }
                    if (confirm) indices.insert(i);
                }
                size_t cursor = 0;
                for (size_t i = 0; i < indices.size(); ++i) {
                    for (Iterator it = value.begin(); it != value.end(); ++it) {
                        (void)(*it);
                        erase(indices[i] - cursor);
                    }
                    indices[i] -= cursor;
                    cursor += value.size();
                }
                cursor = 0;
                for (size_t i = 0; i < indices.size(); ++i) {
                    insert(value2, indices[i] + cursor);
                    cursor += value2.size();
                }
                return indices.size();
            }
            return false;
        }

        bool replace(const Type &value, const Type &value2, size_t start, size_t end, size_t start2 = 0) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
                start2 = start2 % this->count;
            }
            return replace(value, value2, start, end, start2, value.size());
        }

        bool replace(const Type &value, const Type &value2, size_t start = 0) {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return replace(value, value2, start, this->count, 0, value.size());
        }

        bool replaceFirst(const Type &value, const Type &value2, size_t start, size_t end, size_t start2, size_t end2) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
                start2 = start2 % this->count;
                end2 = end2 % this->count + 1;
            }
            if (this->count == 0) return false;
            if (start >= this->count || end > this->count) {
            } else {
                size_t qIndex;
                bool confirm;
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    qIndex = 0;
                    confirm = true;
                    for (JUTIL_INIT(size_t ii, start2); ii < end2; ++ii) {
                        if (i + qIndex >= this->count || *(this->block + i + qIndex) != value[ii]) {
                            confirm = false;
                            break;
                        }
                        ++qIndex;
                    }
                    if (confirm) {
                        for (Iterator it = value.begin(); it != value.end(); ++it) {
                            (void)(*it);
                            erase(i);
                        }
                        insert(value2, i);
                        return true;
                    }
                }
            }
            return false;
        }

        bool replaceFirst(const Type &value, const Type &value2, size_t start, size_t end, size_t start2 = 0) {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
                start2 = start2 % this->count;
            }
            return replaceFirst(value, value2, start, end, start2, value.size());
        }

        bool replaceFirst(const Type &value, const Type &value2, size_t start = 0) {
            return replaceFirst(value, value2, start, this->count, 0, value.size());
        }

        void setIndexingStrategy(IndexingStrategy s) {
            is = s;
        }

        const IndexingStrategy &getIndexingStrategy() const {
            return is;
        }

        Queue<size_t, Allocator> findAll(const ValueType &value, size_t start, size_t end) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
                end = end % this->count + 1;
            }
            Queue<size_t, Allocator> r;
            if (this->count == 0) return r;
            if (start >= this->count || end > this->count) {
            } else {
                for (JUTIL_INIT(size_t i, start); i < end; ++i) {
                    if (value == *(this->block + i)) r.insert(i);
                }
            }
            return r;
        }

        Queue<size_t, Allocator> findAll(const ValueType &value, size_t start = 0) JUTIL_C_ {
            if (is == CYCLICAL) {
                start = start % this->count;
            }
            return findAll(value, start, this->count);
        }

        /**
            @return     A Queue with the same elements as the calling Queue in reverse order.
        */
        Type reverse() JUTIL_C_ {
            Type r;
            r.reserve(this->count);
            for (Iterator i = this->end() - 1; i != this->begin() - 1; --i) r.insert(*i);
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
        bool reallocate(size_t amount) {
            void *nBlock = this->realloc(static_cast<void *>(this->block), sizeof(ValueType) * amount);
            bool succeed = false;
            if (nBlock) {
                this->block = static_cast<ValueType *>(nBlock);
                allocated = amount;
                succeed = true;
            } else {
                nBlock = this->alloc(sizeof(ValueType) * amount);
                if (nBlock) {
                    if (this->copy(nBlock, this->block, (allocated > amount ? amount : allocated) * sizeof(ValueType))) {
                        succeed = true;
                        this->free(this->block);
                        this->block = static_cast<ValueType *>(nBlock);
                        allocated = amount;
                    }
                }
            }
            if (allocated < this->count) this->count = allocated;
            return succeed;
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

        ValueType *getArray() {
            return this->block;
        }

        const ValueType *getArray() const {
            return this->block;
        }

        Type &sort(void (*sorter)(Type *)) {
            sorter(this);
            return *this;
        }

        Type &sort(unsigned char mode = ASCENDING) {
            return sort(mode ? queue_sort::__queueInternalSortAscending<ValueType> : queue_sort::__queueInternalSortDescending<ValueType>);
        }

        Type &resize(size_t s) {
            this->count = s;
            return *this;
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
        Type &operator=(const Type &q) {
            destroy();
            insert(q);
            return *this;
        }

        /**
            Find and element at a specified index.

            @param      Index of element to return.
            @return     Reference to element at the specified index.
        */
        ValueType &operator[](const size_t &n) JUTIL_OVERRIDE_ {
            if (n >= this->count) {
                if (is == CYCLICAL) return this->block[n % this->count];
            } else
                return this->block[n];
            return JUTIL_NULLREF(ValueType);
        }

        /**
            Const overload of @see operator[]
        */
        const ValueType &operator[](const size_t &n) JUTIL_CO_ {
            if (n >= this->count) {
                if (is == CYCLICAL) return this->block[n % this->count];
            } else
                return this->block[n];
            return JUTIL_NULLREF(ValueType);
        }

        /**
            Comparison operator between two Queues.

            @param other    Queue to compare against.
            @return         Wheather or not the Queues are equivelent.
        */
        bool operator==(const Type &other) JUTIL_C_ {
            if (this->count == other.count) {
                for (size_t i = 0; i < this->count; ++i)
                    if ((*this)[i] != other[i]) return false;
                return true;
            } else
                return false;
        }

        /**
            Negation of @see operator==
        */
        JUTIL_CX_ bool operator!=(const Type &other) JUTIL_C_ {
            return !((*this) == other);
        }

        /**
            Conversion operator to @see @class String.
        */
        operator StringBase<char>();

        /**
            Const overload of @see operator String
        */
        operator const StringBase<char>() const;

        template <
            typename U
#ifdef JUTIL_CPP11
            ,
            typename = typename Enable<Convert<U, T>::Value>::Type
#endif
            >
        operator Queue<U, Allocator>() {
            Queue<U, Allocator> r;
#ifdef JUTIL_CPP11
            for (auto &i : *this) r.insert(static_cast<U>(i));
#else
            for (size_t i = 0; i < this->count; ++i) r.insert(static_cast<U>(this->block[i]));
#endif
            return r;
        }

/** =========================================================================================================================================

                C++11 MOVE SEMANTICS

         ** =========================================================================================================================================
        */
#ifdef JUTIL_CPP11

        Type &operator=(Type &&q) {
            this->clear();
            insert(move(q));
            return *this;
        }

        Type &insert(ValueType &&value) {
            ++(this->count);
            if (allocated < this->count) reallocate(this->count << 1);
            JUTIL_NEW(this->block + (this->count - 1), ValueType(move(value)));
            return *this;
        }

        Type &insert(ValueType &&value, const size_t &n) {
            if (n > this->count) {
                if (is == CYCLICAL) return insert(move(value), n % this->count);
            } else {
                if (n == this->count)
                    return insert(move(value));
                else {
                    ++(this->count);
                    if (allocated < this->count) reallocate(this->count << 1);
                    this->move(this->block + n + 1, this->block + n, sizeof(ValueType) * (this->count - n));
                    JUTIL_NEW(this->block + n, ValueType(move(value)));
                }
            }
            return *this;
        }

#endif

        void destroy() JUTIL_OVERRIDE_ {
            for (unsigned i = 0; i < this->count; ++i) (this->block + i)->~ValueType();
            this->free(this->block);
            this->block = JUTIL_NULLPTR;
            this->allocated = 0;
            this->count = 0;
        }

        virtual ~Queue() {
            destroy();
        }

       private:
        IndexingStrategy is;
        size_t allocated;
    };

}  // namespace 

#endif  // JUTIL_QUEUE_H
