#ifndef JUTIL_QUEUE_H
#define JUTIL_QUEUE_H

#include "ContiguousContainer.hpp"
#include "Core/String.h"

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

    template <typename T>
    class Queue : public ContiguousContainer<size_t, T, Queue<T> >, public StringInterface {
    public:

        typedef ContiguousContainer<size_t, T, Queue<T> > BaseType;
        typedef typename BaseType::ValueType ValueType;
        typedef typename BaseType::Iterator Iterator;
        typedef Queue<ValueType> Type;

        #ifdef JUTIL_CPP11
            Queue(const std::initializer_list<ValueType> &list) : BaseType() {
                for (auto i: list) {
                    insert(i);
                }
            }
            Queue(Queue<T> &&q) {
                insert(move(q));
            }
            Queue<T> &operator=(Queue<T> &&q) {
                this->reset();
                insert(move(q));
                return *this;
            }
        #endif

        JUTIL_CX_ Queue() JUTIL_N_ : BaseType() {}

        Queue(const Queue<T> &q) {
            insert(q);
        }

        Queue<T> &operator=(const Queue<T> &q) {
            this->reset();
            insert(q);
            return *this;
        }

        ValueType &operator[](const size_t &n) JUTIL_OVERRIDE_ {
            return BaseType::block[n];
        }


        const ValueType &operator[](const size_t &n) JUTIL_CO_ {
            return BaseType::block[n];
        }

        Type &erase(const size_t &n) JUTIL_OVERRIDE_ {
            ValueType *nBlock = new ValueType[--(this->count)];
            for (size_t i = 0; i < this->count; ++i) {
                nBlock[i] = (*this)[i + (i < n? 0 : 1)];
            }
            this->free();
            this->block = nBlock;
            return *this;
        }

        Iterator erase(Iterator it) {
            for (size_t i = 0; i < this->count; ++i) {
                if (it == &(*this)[i]) {
                    Iterator n = this->begin() + i + 1;
                    erase(i);
                    return n;
                }
            }
            return it;
        }

        Type &insert(const ValueType &value, const size_t &n) JUTIL_OVERRIDE_ {
            if (n == this->size()) {
                return insert(value);
            } else {
                ValueType *nBlock = new ValueType[++(this->count)];
                for (size_t i = this->count; i > 0; --i) {
                    size_t target = i - 1;
                    nBlock[target] = (target == n? value :
                    (target > n? (*this)[target - 1] : (*this)[target]));
                }
                this->free();
                this->block = nBlock;
            }
            return *this;
        }

        Type &insert(const ValueType &value) {
            ValueType *nBlock = new ValueType[++(this->count)]();
            for (size_t i = 0; i < this->count - 1; ++i) {
                ValueType v = this->block[i];
                nBlock[i] = v;
            }
            nBlock[this->count - 1] = value;
            this->free();
            this->block = nBlock;
            return *this;
        }

        template <
            typename U
            #ifdef JUTIL_CPP11
            , typename = typename Enable<Convert<U, T>::Value>::Type
            #endif
        >
        Type &insert(const Queue<U> &q) {
            for (Iterator i = q.begin(); i != q.end(); ++i) {
                insert(static_cast<T>(*i));
            }
            return *this;
        }

        template <
            typename U
            #ifdef JUTIL_CPP11
            , typename = typename Enable<Convert<U, T>::Value>::Type
            #endif
        >
        Type &insert(const Queue<U> &q, const size_t &n) {
            for (Iterator i = q.begin(); i != q.end(); ++i) {
                insert(static_cast<T>(*i), n);
            }
            return *this;
        }

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

        JUTIL_CX_ bool operator !=(const Type &other) JUTIL_C_ {
            return !((*this) == other);
        }

        const size_t find(const ValueType &value) JUTIL_C_ {
            /*for (size_t i = 0; i < this->count; ++i) {
                if ((*this)[i] == value) {
                    return i;
                }
            }*/
            return this->size();
        }

        Queue<size_t> findAll(const ValueType &value) {
            Queue<T> r;
            for (size_t i = 0; i < this->count; ++i) {
                if ((*this)[i] == value) {
                    r.insert((*this)[i]);
                }
            }
            return r;
        }

        Queue<T> reverse() JUTIL_CO_ {
            Queue<T> r;
            for (Iterator i = this->end() - 1; i != this->begin() - 1; --i) {
                r.insert(*i);
            }
            return r;
        }

        Type &clear() {
            this->reset();
            return *this;
        }

        operator String() JUTIL_OVERRIDE_ {
            return asString();
        }

        operator const String() JUTIL_CO_ {
            return asString();
        }

        virtual ~Queue() {}

    private:

        const String asString() JUTIL_C_ {
            String s = "{";
            for (Iterator i = this->begin(); i != this->end(); ++i) {
                s += String::toString(*i) + ", ";
            }
            s = s.substr(0, -3);
            s += "}";
            return s;
        }

    };
}

#endif // JUTIL_QUEUE_H
