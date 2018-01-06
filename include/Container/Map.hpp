#ifndef JUTIL_MAP_H
#define JUTIL_MAP_H

#include "Core/Pair.hpp"
#include "NonContiguousContainer.hpp"

#include <initializer_list>

#ifdef JUTIL_ERR

    #define MAPERR JUTIL_ERR_MAP

    #define MAPERR_OUTER               0x0015

#else
    #define LISTERR_INDEX_INVOKE
    #define LISTERR_ITERATOR_INVOKE
#endif

namespace jutil JUTIL_PUBLIC_ {

    JUTIL_FORWARD_TEMPLATE_2
    class JUTIL_PUBLIC_ Map;

    template<
        typename K,
        typename T
    >
    class JUTIL_PRIVATE_ __MapInternalIterator : public __NonContiguousContainerInternalIterator<K, T, Map<K, T>, __MapInternalIterator<K, T> > {
    public:

        typedef __MapInternalIterator<K, T> Type;
        typedef __NonContiguousContainerInternalIterator<K, T, Map<K, T>, Type> BaseType;
        typedef __NonContiguousContainerInternalNode<K, T, Map<K, T>, Type> Node;

        using BaseType::element;

        JUTIL_CX_ __MapInternalIterator(Node *n) : BaseType(n) {}
        JUTIL_CX_ __MapInternalIterator(const BaseType &i) : BaseType(i) {}
        JUTIL_CX_ __MapInternalIterator(const Type &i) : BaseType(i) {}
        JUTIL_CX_ __MapInternalIterator() : BaseType() {}
        #ifdef JUTIL_CPP11
            JUTIL_CX_ __MapInternalIterator(Type&& i) : BaseType(move(i)) {}
        #endif

        K &key() const noexcept {return element->key;}
        T &value() const noexcept {return element->value;}

        T &operator *() const noexcept {return value();}
        K &operator &() const noexcept {return key();}

        virtual ~__MapInternalIterator() {}
    };

    template<
        typename K,
        typename T
    >
    class JUTIL_PUBLIC_ Map : public NonContiguousContainer<K, T, Map<K, T>, __MapInternalIterator<K, T> > {
    public:

        typedef  __MapInternalIterator<K, T> Iterator;
        typedef NonContiguousContainer<K, T, Map<K, T>, Iterator> BaseType;
        typedef Map<K, T> Type;
        typedef std::initializer_list<Pair<K, T> > Literal;

        Map() : BaseType() {}

        Map(const Map<K, T> &m) {
            for (auto it = m.begin(); it != m.end(); ++it) {
                insert(&it, *it);
            }
        }

        Map(Literal &&dat) {
            for (jutil::Pair<K, T> i: dat) {
                insert(i.first(), i.second());
            }
        }

        Map<K, T> &operator=(const Map<K, T> &m) {
            this->clear();
            for (auto it = m.begin(); it != m.end(); ++it) {
                insert(&it, *it);
            }
            return *this;
        }

        bool find(K k, T v) JUTIL_C_ {
            for (auto &it: *this) {
                if ((&it == k) && (*it == v)) {
                    return true;
                }
            }
            return false;
        }

        bool findByKey(K k, T *t = JUTIL_NULLPTR) JUTIL_C_ {
            for (auto &it: *this) {
                if (&it == k) {
                    if (t) {
                        t = *it;
                    }
                    return true;
                }
            }
            return false;
        }

        bool findByValue(T v, K *k = JUTIL_NULLPTR) JUTIL_C_ {
            for (auto &it: *this) {
                if (*it == v) {
                    if (k) {
                        k = &it;
                    }
                    return true;
                }
            }
            return false;
        }

        #ifdef JUTIL_CPP11
            Map(Map<K, T> &&m) {
                for (auto it = m.begin(); it != m.end(); ++it) {
                    insert(&it, *it);
                }
                m.clear();
            }

            Map<K, T> &operator=(Map<K, T> &&m) {
                this->clear();
                for (auto it = m.begin(); it != m.end(); ++it) {
                    insert(&it, *it);
                }
                m.clear();
                return *this;
            }
        #endif

        virtual Type &insert(const K &key, const T &value) {
            BaseType::insert(value, key);
            return *this;
        }

        constexpr bool operator==(const Type &m) const noexcept {
            if (m.size() == this->size()) {
                for (auto it = this->begin(); it != this->end(); ++it) {
                    if (m.find(*it) != &it) {
                        return false;
                    }
                }
            } else {
                return false;
            }
            return true;
        }

        Map<T, K> reverse() const {
            Map<T, K> r;
            for (auto &i: *this) {
                r.insert(*i, &i);
            }
            return r;
        }

        constexpr bool operator!=(const Type &m) {
            return !(m == (*this));
        }

        virtual ~Map() {}
    };
}

#endif // MAP_H
