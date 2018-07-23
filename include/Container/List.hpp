#ifndef JUTIL_LIST_H
#define JUTIL_LIST_H

#include "Core/Macro.h"
#include "Core/String.h"

#ifdef JUTIL_CPP11
    #include <initializer_list>
#endif

#include "Core/Error.h"
#include "NonContiguousContainer.hpp"

#ifdef JUTIL_ERR

    #define LISTERR JUTIL_ERR_LIST

    #define LISTERR_OUTER               0xffff
    #define LISTERR_INNER_INDEX         0x0000
    #define LISTERR_INNER_ITERATOR      0x0001
    #define LISTERR_INDEX_MSG           "Invalid index"
    #define LISTERR_ITERATOR_MSG        "Invalid iterator"
    #define LISTERR_INDEX_INVOKE        JUTIL_INVOKE_ERR(LISTERR_OUTER, LISTERR_INNER_INDEX, JUTIL_ERR_MSG(LISTERR_INDEX_MSG))
    #define LISTERR_ITERATOR_INVOKE     JUTIL_INVOKE_ERR(LISTERR_OUTER, LISTERR_INNER_ITERATOR, JUTIL_ERR_MSG(LISTERR_ITERATOR_MSG))

#else
    #define LISTERR_INDEX_INVOKE
    #define LISTERR_ITERATOR_INVOKE
#endif

#ifdef JUTIL_CPP11
    #define JUTIL_FOREACH(a, b)             for (auto (a) = (b).begin(); (a) != (b).end(); ++(a))
    #define JUTIL_REVERSE_FOREACH(a, b)     for (auto (a) = (b).begin() + ((b).size() - 1); (a).get(); --(a))
    #define JUTIL_FOREACH_(a, b)            JUTIL_FOREACH(a, b)
    #define JUTIL_REVERSE_FOREACH_(a, b)    JUTIL_REVERSE_FOREACH(a, b)
#else
    #define JUTIL_FOREACH_(a, b)            for (Iterator a = (b).begin(); a != (b).end(); ++a)
    #define JUTIL_REVERSE_FOREACH_(a, b)    for (Iterator a = (b).begin() + ((b).size() - 1); a != (b).end(); --a)
    #define JUTIL_FOREACH(a, b, c)          for (jutil::List<a>::Iterator (b) = (c).begin(); (b) != (c).end(); ++(b))
    #define JUTIL_REVERSE_FOREACH(a, b, c)  for (jutil::List<a>::Iterator (b) = (c).begin() + ((c).size() - 1); (b) != (c).end(); --(b))
#endif

#ifdef JUTIL_MACRO
    #define JLIC(a)                                     JUTIL_CX_ bool operator a(const Iterator &it) JUTIL_CN_ {return sameList(it.node) && (node->index a it.index);}
    #define JUTIL_LIST_GENERATE_ITERATOR_COMPARATORS    JUTIL_CALL_UNROLL(JLIC, >, <, >=, <=)
#endif

namespace jutil JUTIL_PUBLIC_ {

    template <typename T>
    class JUTIL_PRIVATE_ __ListInternalIterator : public __NonContiguousContainerInternalIterator<size_t, T, List<T>, __ListInternalIterator<T> > {
    public:

        typedef __ListInternalIterator<T> Type;
        typedef __NonContiguousContainerInternalIterator<size_t, T, List<T>, Type> BaseType;

        using BaseType::element;

        JUTIL_CX_ __ListInternalIterator(__NonContiguousContainerInternalNode<size_t, T, List<T>, Type> *n) : BaseType(n) {}
        JUTIL_CX_ __ListInternalIterator(const __NonContiguousContainerInternalIterator<size_t, T, List<T>, Type> &i) : BaseType(i) {}
        JUTIL_CX_ __ListInternalIterator(const Type &i) : BaseType(i) {}
        JUTIL_CX_ __ListInternalIterator() : BaseType() {}
        #ifdef JUTIL_CPP11
            JUTIL_CX_ __ListInternalIterator(Type&& i) : BaseType(move(i)) {}
        #endif

        JUTIL_CX_ T &operator*() JUTIL_CN_ {
            return element->value;
        }

        JUTIL_CX_ T *operator->() JUTIL_CN_ {
            return &(element->value);
        }

        typename __NonContiguousContainerInternalIterator<size_t, T, List<T>, __ListInternalIterator<T> >::Node *get() {
            return element;
        }

    private:

    };

    template <typename T>
    class JUTIL_PUBLIC_ List : public NonContiguousContainer<size_t, T, List<T>, __ListInternalIterator<T> >, public StringInterface {
    public:

        typedef NonContiguousContainer<size_t, T, List<T>, __ListInternalIterator<T> > BaseType;
        typedef __ListInternalIterator<T> Iterator;
        typedef typename BaseType::Node Node;
        typedef Node* Element;

        JUTIL_GENERATE_TEMPLATE_ALIASES(T);

        #ifdef JUTIL_CPP11
            using Literal =     std::initializer_list<Type>;
            using Literal2d =   std::initializer_list<Literal>;
            using Literal3d =   std::initializer_list<Literal2d>;
            using Literal4d =   std::initializer_list<Literal3d>;
        #endif

        using BaseType::head;
        using BaseType::length;
        using BaseType::endNode;
        using BaseType::seek;
        using BaseType::size;
        using BaseType::validIterator;
        using BaseType::seekToFirst;
        using BaseType::end;

        /** =========================================================================================================================================
        */
                    /** @section CONSTRUCTORS

                            List()                  Construct a List with 0 elements.
                            List(size_t n)          Construct a List with [n] elements, all initialized with the default value of Type.
                            List(size_t n, Type t)  Construct a List with [n] elements, all initialized with value [t].
                            List(Type[] arr)        Construct a List from an array. The array's length will be deduced at compile-time.

                            If C++11 is enabled, std::initializer_lists (Literals) can also be used to construct a list.
                            e.g. List<Type> l = {a, b, c, d, e, f};
                            e.g. List<List<Type> > =  {{a, b}, {c, d}, {e, f}};

                            Copy and rvlaue-move constructors are included.

                            Elements are created using dynamically allocated Node pointers.

                            Elements are deallocated either by the destructor, the erase(size_t i) method,
                            by Iterator::change(Element node), or by Iterator's deconstructor.

                            Iterator only dallocates elements who have been removed from a list.

                            Elements are NOT consecutive in memory.
                    */
        /** =========================================================================================================================================
        */
        JUTIL_CX_ List() JUTIL_N_ : BaseType() {}
        List(size_t n, const_Reference dat) JUTIL_N_ : BaseType() {
            for (JUTIL_INIT(size_t i, 0); i < n; ++i) {
                this->insert(dat);
            }
        }
        #ifdef JUTIL_CPP11
            JUTIL_CX_ List(size_t n) JUTIL_N_ : List{n, {}} {}

            List(Literal &&init) JUTIL_N_ : BaseType() {
                for (auto &i: init) {
                    this->insert(i);
                }
            }
            List(Literal2d &&init) JUTIL_N_ : BaseType() {
                JUTIL_FOREACH_(it, init) {
                    insert(List<Type>(*it));
                }
            }
            List(Literal3d &&init) JUTIL_N_ : BaseType() {
                JUTIL_FOREACH_(i, init) {
                    JUTIL_FOREACH_(j, *i) {
                        insert(List<Type>(*j));
                    }
                }
            }
            List(Literal4d &&init) JUTIL_N_ : BaseType() {
                JUTIL_FOREACH_(i, init) {
                    JUTIL_FOREACH_(j, *i) {
                        JUTIL_FOREACH_(k, *j) {
                            insert(List<Type>(*k));
                        }
                    }
                }
            }
            List(List<Type> &&list) JUTIL_N_ : BaseType() {
                length = list.size();
                head = list.head;
                endNode = list.endNode;
                list.head = JUTIL_NULLPTR;
                list.length = 0;
                list.endNode = JUTIL_NULLPTR;
            }
        #else
            List(size_t n) JUTIL_N_ : BaseType() {
                length = n;
                for (JUTIL_INIT(size_t i, 0); i < n; ++i) {
                    insert(Type());
                }
            }
        #endif
        template<size_t size>
        List(const Type (&arr)[size]) : BaseType() {
            for (JUTIL_INIT(size_t i, 0); i < size; ++i) {
                insert(arr[i]);
            }
        }
        List(const List<Type> &list) JUTIL_N_ : BaseType() {
            for (auto &i: list) {
                insert(i);
            }
        }

        /** =========================================================================================================================================
        */
                    /** @section OPERATORS

                            operator[](size_t i)    return a reference to the element data at position [i].
                            operator[](Iterator it) return reference to the element data pointed to by iterator [it].

                            Const overloads are included for both operator[]() methods.

                            Assignment and rvalue-move operators are included.
                            Both return a reference to the List<Type> object on which they are called.

                            Conversion operator is included for implicitly converting between Lists of different types.
                    */
        /** =========================================================================================================================================
        */

        virtual Reference operator[](const size_t &i) JUTIL_OVERRIDE_ {
            if (seek(i) && head) {
                return head->value;
            } else {
                LISTERR_INDEX_INVOKE;
                return JUTIL_NULLREF(Type);
            }
        }
        virtual Reference operator[](const Iterator &it) {
            if (BaseType::validIterator(it)) {
                return (*it);
            } else {
                LISTERR_ITERATOR_INVOKE;
                return JUTIL_NULLREF(Type);
            }
        }
        virtual const_Reference operator[](const size_t &i) JUTIL_CO_ {
            JUTIL_INIT(Element n, head);
            n = seek(n, i);
            return n->value;
        }
        virtual const_Reference operator[](const Iterator &it) JUTIL_C_ {
            if (BaseType::validIterator(it)) {
                return (*it);
            } else {
                LISTERR_ITERATOR_INVOKE;
                return JUTIL_NULLREF(Type);
            }
        }
        virtual List<Type> &operator=(const List<Type> &list) JUTIL_N_ {
            this->clear();
            for (auto &i: list) {
                insert(i);
            }
            return *this;
        }

        #ifdef JUTIL_CPP11
            virtual List<Type> &operator=(List<Type> &&list) {
                if (this != &list) {
                    this->clear();
                    length = list.size();
                    for (auto &i: list) {
                        insert(i);
                    }
                    list.clear();
                }
                return *this;
            }
        #endif
        template <typename U>
        JUTIL_CX_ operator List<U>() JUTIL_N_ {
            List<U> result;
            JUTIL_FOREACH_(it, *this) {
                result.insert(static_cast<U>(*it));
            }
            return result;
        }

        operator String() {
            String s = "{";
            for (Iterator i = this->begin(); i != this->end(); ++i) {
                s += String::toString(*i) + ", ";
            }
            s = s.substr(0, -3);
            s += "}";
            return s;
        }

        operator const String() const {
            String s = "{";
            for (Iterator i = this->begin(); i != this->end(); ++i) {
                s += String::toString(*i) + ", ";
            }
            s = s.substr(0, -3);
            s += "}";
            return s;
        }

        /** =========================================================================================================================================
        */
                    /** @section INSERT

                            These insertion methods can be used to add new elements to the list.

                            insert()                        insert 1 new element with the default value of Type.
                            insert(Type t)                  insert 1 new element with the sepcified value of [t].
                            insert(Type t, size_t i)        insert [i] new elements with the sepcified value of [t].
                            insert(List<Type> l, size_t i)  insert all of the elements of [l] at position p.
                            insert(List<Type> l)            insert all of the elements of [l] at the end of the list.

                            Attempting to insert an element at the position after the last element (length()) will add the element
                            at the end of the list.

                            Inserted elements are stored as a dynamically allocated Node pointer (Element).

                            All of these methods return a referecne to the List<Type> object on which they are called.
                    */
        /** =========================================================================================================================================
        */

        List<Type> &insert() JUTIL_N_ {
            #ifdef JUTIL_CPP11
                return insert({}, length);
            #else
                return Insert(Type(), length);
            #endif
        }
        List<Type> &insert(const_Reference t) JUTIL_N_ {
            if (length == 0) {
                head = new Node(this, JUTIL_NULLPTR, endNode, t, 0);
            } else if (seek(length - 1)) {
                head->next = new Node(this, head, endNode, t, head->key + 1);
                head = head->next;
            }
            endNode->previous = head;
            ++length;
            return *this;
        }
        List<Type> &insert(const_Reference t, const size_t &i) JUTIL_OVERRIDE_ {
            if (i == length) {
                return insert(t);
            } else if (seek(i)) {
                JUTIL_INIT(Element o, head);
                ++(head->key);
                while (head->next) {
                    head = head->next;
                    ++(head->key);
                }
                Element d = new Node(this, o->previous, o, t, i);
                JUTIL_INIT(Element op, o->previous);
                o->previous = d;
                if (op) {
                    op->next = d;
                }
                ++length;
            } else {
                LISTERR_INDEX_INVOKE;
            }
            return *this;
        }
        List<Type> &insert(const List<Type> &list, size_t p) {
            JUTIL_INIT(List<Type> l, list.reverse());
            JUTIL_FOREACH_(it, l) {
                insert(*it, p);
            }
            return *this;
        }
        List<Type> &insert(const List<Type> &list) JUTIL_N_ {return insert(list, length);}
        #ifdef JUTIL_CPP11
            List<Type> &insert(const Literal &init) noexcept {
                List<Type> list(init);
                return insert(list);
            }
        #endif
        /** =========================================================================================================================================
        */
                    /** @section ERASE

                            These erasal methods can be used to remove elements from the list.

                            erase()                         Remove the last element of the list. CALLS erase(size_t)
                            erase(size_t i)                 Remove the element at position [i]. The elemnt's memory is deallocated.
                            erase(Iterator it)              Remove the element which the iterator [it] points at.
                            eraseFirst(Type t)              Remove the first instance of [t] from the list. CALLS erase(size_t)
                            eraseLast(Type t)               Remove the last instance of [t] from the list. CALLS erase(size_t)
                            eraseAll(Type t)                Remove all instances of [t] from the list. CALLS erase(size_t i)
                            eraseAfter(Type t, size_t i)    Remove the first instance of [t] after position [i]. CALLS erase(size_t)
                            eraseAfter(Type t, Iterator it) Remove the first instance of [t] after the element pointed at by [it]. CALLS erase(size_t)

                            All of these methods return a referecne to the List<Type> object on which they are called.
                    */
        /** =========================================================================================================================================
        */
        List<T> &erase(const size_t &i) JUTIL_OVERRIDE_ {
            if (Element d = deattach(i)) {
                JUTIL_FREE_(d);
            }
            return *this;
        }
        List<T> &erase(const Iterator &it) {
            if (validIterator(it)) {
                if (Element d = deattach(it.element->key)) {
                    d->flags |= 0x01;
                }
            } else {
                LISTERR_ITERATOR_INVOKE;
            }
            return *this;
        }
        List<Type> &erase() {return erase(length - 1);}
        List<Type> &eraseFirst(const_Reference t) {
            JUTIL_INIT(size_t p, 0);
            if (find(t, &p)) {
                return erase(p);
            }
            return *this;
        }
        List<Type> &eraseLast(const_Reference t) {
            JUTIL_INIT(size_t p, 0);
            if (findLast(t, &p)) {
                return erase(p);
            }
            return *this;
        }
        List<Type> &eraseAfter(const_Reference t, size_t i) {
            JUTIL_INIT(size_t p, 0);
            if (findAfter(t, i, &p)) {
                return erase(p);
            }
            return *this;
        }
        List<Type> &eraseAfter(const_Reference t, const Iterator &it) {
            if (validIterator(it)) {
                JUTIL_INIT(size_t p, 0);
                if (findAfter(t, it, &p)) {
                    return erase(p);
                }
            } else {
                LISTERR_ITERATOR_INVOKE;
            }
            return *this;
        }
        List<Type> &eraseAll(const_Reference t) {
            JUTIL_INIT(size_t p, 0);
            while (find(t, &p)) {
                erase(p);
            }
            return *this;
        }
        List<Type> &truncate(size_t p) {
            while (p + 1 < length) {
                erase(p + 1);
            }
            return *this;
        }
        /** =========================================================================================================================================
        */
                    /** @section FIND

                            These methods can be used to find specific Type values in the list.

                            ALL of these methods return true if the value Type [t] is found within the list.
                            They also set the data at memory address size_t [p] to the position found.
                            If [t] is not found, [p] is set as the length of the array.

                            find(Type t, size_t p)                  [p] = The position of the first of instance of [t].
                            findLast(Type t, size_t p)              [p] = The position of the last of instance of [t].
                            findAfter(Type t, size_t i, size_t p)   [p] = The position of the first of instance of [t] after position [i].
                    */
        /** =========================================================================================================================================
        */
        JUTIL_CX_ bool find(const_Reference t, size_t *p) JUTIL_CN_ {
            for (JUTIL_INIT(size_t i, 0); i < length; ++i) {
                if ((*this)[i] == t) {
                    if (p) {
                        *p = i;
                    }
                    return true;
                }
            }
            if (p) {
                *p = length;
            }
            return false;
        }

        JUTIL_CX_ bool findLast(const_Reference t, size_t *p) JUTIL_CN_ {
            JUTIL_INIT(long long lp, -1);
            for (JUTIL_INIT(size_t i, 0); i < length; ++i) {
                if ((*this)[i] == t) {
                    lp = i;
                }
            }
            if (lp == -1) {
                if (p) {
                    *p = length;
                }
                return false;
            } else {
                if (p) {
                    *p = lp;
                }
                return true;
            }
        }
        JUTIL_CX_ bool findAfter(const_Reference t, size_t i, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            ++i;
            if (i >= length) {
                if (p) {
                    *p = length;
                }
                LISTERR_INDEX_INVOKE;
                return false;
            } else {
                for (JUTIL_INIT(size_t j, i); j < length; ++j) {
                    if ((*this)[j] == t) {
                        if (p) {
                            *p = i;
                        }
                        return true;
                    }
                }
                if (p) {
                    *p = length;
                }
                return false;
            }
        }
        JUTIL_CX_ bool findAfter(const_Reference t, const Iterator &it, size_t *p = JUTIL_NULLPTR) JUTIL_C_ {
            if (validIterator(it)) {
                return findAfter(t, it.index, p);
            } else {
                LISTERR_ITERATOR_INVOKE;
                return false;
            }
        }
        JUTIL_C_ jutil::List<size_t> replace(const_Reference a, const_Reference b) {
            jutil::List<size_t> r;
            for (JUTIL_INIT(size_t i, 0); i < length; ++i) {
                if ((*this)[i] == a) {
                    (*this)[i] = b;
                    r.insert(i);
                }
            }
            return r;
        }
        JUTIL_CX_ size_t count(const_Reference v) JUTIL_C_ {
            size_t r = 0;
            for (auto &it: *this) {
                if (*it == v) {
                    ++r;
                }
            }
            return r;
        }
        /** =========================================================================================================================================
        */
                    /** @section DATA

                            These methods return information about the list or its elements.

                            empty()             Returns true if the list contains no elements. False otherwise.
                            size()              Returns the number of elements in the list as a size_t.
                            first()             Returns a reference to the data of the first element in the list.
                            last()              Returns a reference to the data of the last element in the list
                            array(Type[] arr)   VOID. Fills the array [arr] with the data of each element in the list.

                            @note

                            array(Type[])       Will NOT access references or pointers to any data. it gets the VALUES of the elements in the list.
                    */
        /** =========================================================================================================================================
        */
        JUTIL_CX_ const bool empty() JUTIL_CNO_ {return length == 0;}
        Reference first() JUTIL_CN_ {
            JUTIL_INIT(Element n, head);
            n = seek(n, 0);
            return n->value;
        }
        Reference last() JUTIL_CN_ {
            JUTIL_INIT(Element n, head);
            n = seek(n, length - 1);
            return n->value;
        }
        virtual void array(Type arr[]) JUTIL_CN_ {
            for (JUTIL_INIT(size_t i, 0); i < size(); ++i) {
                arr[i] = (*this)[i];
            }
        }
        /** =========================================================================================================================================
        */
                    /** @section MISC

                        Other methods.

                        resize(size_t s, Type t)    Change the length of the list to [s]. Any new elements generated will be filled with value of [t].
                                                    Returns a reference to the list on which it is called.

                        clear()                     Remove and deallocate of the list's elements.
                                                    Returns a reference to the list on which it is called.

                        reverse()                   Returns the List with it's element's orders reversed.
                    */
        /** =========================================================================================================================================
        */
        #ifdef JUTIL_CPP11
            List<Type> &resize(size_t s, const_Reference t = {})
        #else
            List<Type> &resize(size_t s, const_Reference t = Type())
        #endif
        JUTIL_N_ {
            while (s > length) {
                insert(t);
            }
            while (s < length) {
                erase();
            }
            return *this;
        }
        List<Type> &clear() JUTIL_N_ {
            for (Iterator it = this->begin(); it != this->end(); ++it) {
                erase(it);
            }
            head = endNode;
            return *this;
        }
        JUTIL_CX_ List<Type> reverse() JUTIL_CN_ {
            List<Type> list;
            size_t i = 0;
            for (Iterator it = this->end() - 1; i < this->size(); ++i) {
                list.insert(*it);
                --it;
            }
            return list;
        }
        List<List<Type> > explode(const_Reference val) {
            List<List<Type> > r;
            r.insert(*this);
            size_t rep = 0;
            while (r.last().find(val, JUTIL_NULLPTR)) {
                for (unsigned i = 0; i < r[rep].length; ++i) {
                    if (r[rep][i] == val) {
                        r.insert(List<Type>());
                        for (unsigned ii = i + 1; ii < r[rep].length; ++ii) {
                            r[rep + 1].insert(r[rep][ii]);
                        }
                        r[rep].truncate(i - 1);
                        ++rep;
                        break;
                    }
                }
            }
            return r;
        }
        /** =========================================================================================================================================
        */

        virtual ~List() {}

    private:

        Element deattach(size_t i) {
            if (seek(i)) {
                JUTIL_INIT(Element d, head);
                while (head->next) {
                    head = head->next;
                    --(head->key);
                }
                if (d->previous) {
                    d->previous->next = d->next;
                }
                if (d->next) {
                    d->next->previous = d->previous;
                }

                --length;
                if (length == 0) head = endNode;

                seek(0);

                return d;

            } else {
                LISTERR_INDEX_INVOKE;
                return JUTIL_NULLPTR;
            }
        }

        static Element seek(Element node, size_t i) JUTIL_N_ {
            while (node && (i > node->key)) {
                node = node->next;
            }
            while (node && (i < node->key)) {
                node = node->previous;
            }
            return node;
        }

    };
}

#endif // JUTIL_LIST_H
