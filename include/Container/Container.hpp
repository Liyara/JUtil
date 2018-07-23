#ifndef JUTIL_CONTAINER_H
#define JUTIL_CONTAINER_H

#include "Core/Macro.h"
#ifdef JUTIL_CPP11
    #include "Traits/Traits.hpp"
#endif

namespace jutil JUTIL_PUBLIC_ {

    template <typename D>
    class JUTIL_PUBLIC_ Iterator {
    public:

        typedef D DerivedType;

        virtual DerivedType &operator++() = 0;
        virtual DerivedType &operator--() = 0;
        virtual DerivedType operator+(size_t) JUTIL_C_ = 0;
        virtual DerivedType operator-(size_t) JUTIL_C_ = 0;
    };

    template <
        typename K,
        typename T,
        typename D,
        typename I
    >
    class JUTIL_PUBLIC_ Container {
    public:

        typedef K KeyType;
        typedef T ValueType;
        typedef D DerivedType;
        typedef I DerivedIterator;
        typedef Container<KeyType, ValueType, DerivedType, DerivedIterator> Type;

        virtual DerivedType &insert(const ValueType&, const KeyType&) = 0;
        virtual ValueType &operator[](const KeyType&) = 0;
        virtual const ValueType &operator[](const KeyType&) JUTIL_C_ = 0;
        virtual DerivedType &erase(const KeyType&) = 0;
        virtual const size_t size() JUTIL_C_ = 0;
        virtual const bool empty() JUTIL_C_ = 0;
        virtual DerivedIterator begin() = 0;
        virtual DerivedIterator end() = 0;
        virtual const DerivedIterator begin() JUTIL_C_ = 0;
        virtual const DerivedIterator end() JUTIL_C_ = 0;
        virtual DerivedType &clear() = 0;
    };
}

#endif // JUTIL_CONTAINER_H
