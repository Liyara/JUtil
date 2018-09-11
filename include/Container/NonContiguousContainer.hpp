#ifndef JUTIL_NON_CONTIGUOUS_CONTAINER_H
#define JUTIL_NON_CONTIGUOUS_CONTAINER_H

#include "Container.hpp"
#include "Core/Error.h"

#ifdef JUTIL_ERR

    #define NCCERR JUTIL_ERR_NCC

    #define NCCERR_OUTER               0x0012
    #define NCCERR_INNER_DUPLICATE     0x0000
    #define NCCERR_DUPLICATE_MSG       "Duplicate key"
    #define NCCERR_DUPLICATE_INVOKE    JUTIL_INVOKE_ERR(NCCERR_OUTER, NCCERR_INNER_DUPLICATE, JUTIL_ERR_MSG(NCCERR_DUPLICATE_MSG))

#else
    #define NCCERR_DUPLCATE_INVOKE
#endif

#define JUTIL_NCCII __NonContiguousContainerInternalIterator
#define JUTIL_NCCIN __NonContiguousContainerInternalNode

namespace jutil JUTIL_PUBLIC_ {

    template<
        typename K,
        typename T,
        typename D,
        typename I,
        typename
        #ifdef JUTIL_CPP11
            = typename Enable<BaseOf<JUTIL_NCCII<K, T, D, I>, I>::Value>::Type
        #else
            = T
        #endif
    >
    class JUTIL_PUBLIC_ NonContiguousContainer;

    template<
    typename K,
        typename T,
        typename D,
        typename I
    >
    struct JUTIL_PRIVATE_ JUTIL_NCCIN {

        typedef NonContiguousContainer<K, T, D, I> ContainerType;
        typedef JUTIL_NCCIN<K, T, D, I> Type;
        typedef K KeyType;
        typedef T ValueType;

        JUTIL_CX_ JUTIL_NCCIN(ContainerType * const c, Type *p, Type *n, const ValueType &v, const KeyType &k, unsigned char JUTIL_RVAL_ f  = 0x00) JUTIL_N_ :
            JUTIL_INIT(container, c),
            JUTIL_INIT(next, n),
            JUTIL_INIT(previous, p),
            JUTIL_INIT(key, k),
            JUTIL_INIT(value, v),
            JUTIL_INIT(flags, f) {}
        #ifdef JUTIL_CPP11
            JUTIL_CX_ JUTIL_NCCIN(Type &&n) JUTIL_N_ = delete;
            JUTIL_CX_ JUTIL_NCCIN(const Type &n) JUTIL_N_ = delete;
            JUTIL_CX_ JUTIL_NCCIN() JUTIL_N_ = delete;
        #endif

        ContainerType *container;
        Type *next, *previous;
        KeyType key;
        ValueType value;
        unsigned char flags;

        virtual ~JUTIL_NCCIN() JUTIL_N_ {JUTIL_CALL_UNROLL(JUTIL_NULL_, previous, next, container)}
    };


    template<
        typename K,
        typename T,
        typename D,
        typename I,
        typename //Enforces that I is derivitive of JUTIL_NCCII
    >
    class JUTIL_PUBLIC_ NonContiguousContainer :
        private Container<K, T, D, I> {

    public:

        typedef K KeyType;
        typedef T ValueType;
        typedef D DerivedType;
        typedef I Iterator;
        typedef JUTIL_NCCIN<K, T, D, I> Node;
        typedef NonContiguousContainer<KeyType, ValueType, DerivedType, Iterator> Type;

        NonContiguousContainer() :
            JUTIL_INIT(head, new Node(this, JUTIL_NULLPTR, JUTIL_NULLPTR, JUTIL_DEFAULT(ValueType), JUTIL_DEFAULT(KeyType), 0x02)),
            JUTIL_INIT(endNode, head),
            JUTIL_INIT(length, 0) {}

        virtual const size_t size() JUTIL_CO_ {
            return length;
        }

        virtual const bool empty() JUTIL_CO_ {
            return length == 0;
        }

        virtual DerivedType &insert(const ValueType &value, const KeyType &key) JUTIL_OVERRIDE_ {
            if (seek(key)) {
                NCCERR_DUPLICATE_INVOKE;
            } else {
                if (length == 0) {
                    seekToFirst();
                    head = new Node(this, JUTIL_NULLPTR, endNode, value, key);
                } else {
                    seek(end() - 1);
                    head->next = new Node(this, head, endNode, value, key);
                    head = head->next;
                    endNode->previous = head;

                }
                ++length;
            }
            return *JUTIL_CAST_DERIVED;
        }

        virtual ValueType &operator[](const KeyType &key) JUTIL_OVERRIDE_ {
            seek(key);
            return head->value;
        }

        virtual const ValueType &operator[](const KeyType &key) JUTIL_CO_ {
            return (at(key))->value;
        }

        virtual DerivedType &erase(const KeyType &key) JUTIL_OVERRIDE_ {
            if (seek(key)) {
                if (head->previous) {
                    head->previous->next = head->next;
                }
                if (head->next) {
                    head->next->previous = head->previous;
                }
                --length;
            }
            return *JUTIL_CAST_DERIVED;
        }

        virtual DerivedType &fillArray(ValueType arr[]) JUTIL_C_ {
            ValueType c[length];
            Node *n = head;
            while (n && n->previous) n = n->previous;
            for (unsigned i = 0; i < length; ++i) {
                c[i] = n->value;
                ++n;
            }
            arr = c;
            return *JUTIL_CONST_CAST_DERIVED;
        }

        virtual DerivedType &clear() JUTIL_OVERRIDE_ {
            seekToFirst();
            while (head && head->next) {
                Node *n = head;
                head = head->next;
                if (n && !(n->flags & 0x02)) {
                    delete n;
                    n = JUTIL_NULLPTR;
                }
            }
            length = 0;
            head = NULL;
            return *JUTIL_CAST_DERIVED;
        }

        Iterator begin() JUTIL_OVERRIDE_ {
            seekToFirst();
            return Iterator(head);
        }

        Iterator end() JUTIL_OVERRIDE_ {
            return Iterator(endNode);
        }

        const Iterator begin() JUTIL_CO_ {
            Node *n = head;
            while (n->previous) n = n->previous;
            return Iterator(n);
        }

        const Iterator end() JUTIL_CO_ {
            return Iterator(endNode);
        }

        virtual ~NonContiguousContainer() {
            clear();
            JUTIL_NULL_(head);
            JUTIL_FREE(endNode);
        }

    protected:

        inline void seekToLast() {
            while (head && head->next && !(head->next->flags & 0x02)) head = head->next;
        }

        inline void seekToFirst() {
            while (head && head->previous) head = head->previous;
        }

        Node *lastNode() JUTIL_C_ {
            Node *e = head;
            while (e && e->next && !(e->next->flags & 0x02)) e = e->next;
            return e;
        }

        Node *firstNode() JUTIL_C_ {
            Node *e = head;
            while (head->previous) head = head->previous;
            return e;
        }

        Node *at(const KeyType &key) JUTIL_C_ {
            Node *e = head;
            if (e) {
                while (e && (e->previous)) {
                    e = e->previous;
                }
                while (e && (e->key != key)) {
                    e = e->next;
                }
                return e;
            } else {
                return JUTIL_NULLPTR;
            }
        }

        Node *seek(const KeyType &key) {
            Node *n = at(key);
            if (n && !(n->flags & 0x02)) {
                return (head = n);
            } else {
                return JUTIL_NULLPTR;
            }
        }

        bool seek(Iterator it) {
            if (validIterator(it)) {
                head = it.element;
                return true;
            } else {
                return false;
            }
        }

        JUTIL_CX_ bool validIterator(Iterator it) JUTIL_C_ {
            return (it.element && it.element->container && it.element->container == this && at(it.element->key));
        }

        Node *head;
        Node *endNode;
        size_t length;

        friend class JUTIL_NCCII<K, T, D, I>;
    };

    template<
        typename K,
        typename T,
        typename CD,
        typename D
    >
    class JUTIL_PRIVATE_ JUTIL_NCCII : public Iterator<D> {

        typedef D DerivedType;
        typedef JUTIL_NCCII<K, T, CD, DerivedType> Type;


        public:

            typedef JUTIL_NCCIN<K, T, CD, D> Node;

        JUTIL_NCCII() : element(JUTIL_NULLPTR) {}
        JUTIL_NCCII(Node *n) : element(n) {}
        JUTIL_NCCII(const Type &it) : element(it.element) {}

        virtual DerivedType &operator++() JUTIL_OVERRIDE_ {
            if (element) {
                Node *temp = element;
                if (!(element->flags & 0x02)) {
                    element = element->next;
                }
                if (temp->flags & 0x01) {
                    JUTIL_FREE_(temp);
                }
            }
            return *JUTIL_CAST_DERIVED;
        }

        virtual DerivedType &operator--() JUTIL_OVERRIDE_ {
            if (element) {
                if (element->flags & 0x02) {
                    element = element->container->lastNode();
                } else {
                    Node *temp = element;
                    element = element->previous;
                    if (temp->flags & 0x01) {
                        JUTIL_FREE_(temp);
                    }
                }
            } else {

            }
            return *JUTIL_CAST_DERIVED;
        }

        virtual DerivedType operator+(size_t s) JUTIL_CO_ {
            DerivedType it(*this);
            for (size_t i = 0; i < s; ++i) ++it;
            return it;
        }

        virtual DerivedType operator-(size_t s) JUTIL_CO_ {
            Type it(*this);
            for (size_t i = 0; i < s; ++i) --it;
            return static_cast<DerivedType>(it);
        }

        virtual bool operator==(const Type &i) {
            return (i.element == element);
        }

        virtual bool operator!=(const Type &i) {
            return !((*this) == i);
        }

    protected:
        Node *element;
    }; //END ITERATOR
}

#undef JUTIL_NCCII
#undef JUTIL_NCCIN

#endif // JUTIL_NON_CONTIGUOUS_CONTAINER_H
