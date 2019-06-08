#ifndef JUTIL_STACK_H
#define JUTIL_STACK_H

#include <JUtil/Container/LIFO.hpp>

#define STACK_ERR_INDEX 0x07

namespace jutil {
    template <size_t _capacity, typename T>
    class Stack : public LIFO<T> {
    public:

        typedef LIFO<T> BaseType;
        typedef T ValueType;

        Stack() : _stack(new ValueType[_capacity]), _top(_stack - 1) {}

        virtual ValueType &top() JUTIL_OVERRIDE_ {
            return *_top;
        }

        virtual bool empty() JUTIL_CO_ {
            return _top == _stack - 1;
        }

        size_t capacity() JUTIL_CO_ {
            return _capacity;
        }

        size_t size() JUTIL_CO_ {
            return (_top >= _stack? (_top - _stack) + 1 : 0);
        }

        virtual bool push(const T &val) {
            this->increment();
            if (this->overflow()) return false;
            JUTIL_NEW(&(this->top()), T(val));
            return true;
        }

        virtual bool pop() {
            if (this->empty()) return false;
            this->top().~T();
            this->decrement();
            return true;
        }

        virtual ~Stack() {
            for (size_t i = 0; i < size(); ++i) (_stack + i)->~ValueType();
            ::operator delete[](_stack);
            _stack = NULL;
            _top = NULL;
        }

    private:
        void increment() {
            ++_top;
        }
        void decrement() {
            --_top;
        }
        bool overflow() {
            return _top > _stack + _capacity;
        }
        ValueType *_stack, *_top;
    };
}

#endif // JUTIL_STACK_H
