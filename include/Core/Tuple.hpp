#ifndef JUTIL_TUPLE_H
#define JUTIL_TUPLE_H

#include "Core/Macro.h"
#include "Traits/Traits.hpp"

#ifndef JUTIL_CPP11
    #error The "Tuple.hpp" file in the JUtil library requires C++11 to operate.\
    Please enable C++11 in your compilers settings, or uninclude "Tuple.hpp".
#endif

namespace jutil JUTIL_PUBLIC_ {

    template <size_t index, typename... Types>
    using IndexedType = typename TypeAtIndex<index, Types...>::Type;

    namespace tuple_base JUTIL_PRIVATE_ {
        template<size_t index, typename T>
        struct TupleElement {
            JUTIL_GENERATE_METHODS(TupleElement, default)
            explicit constexpr TupleElement(const T &value) noexcept : Value(value) {}
            explicit constexpr TupleElement(T&& value) noexcept : Value(jutil::move(value)) {}
            T Value;

            virtual ~TupleElement() {}
        };

        template <typename, typename...>
        struct TupleBase;

        template <size_t... indices, typename... Types>
        struct TupleBase<IndexSet<indices...>, Types...> : public TupleElement<indices, Types>... {
            JUTIL_GENERATE_METHODS(TupleBase, default)
            template<
                typename... CTypes,
                typename = typename jutil::Enable<(sizeof... (Types) == sizeof... (CTypes))>::Type
            >
            explicit constexpr TupleBase(CTypes&&... values) noexcept : TupleElement<indices, Types>(jutil::rval<CTypes>(values))... {}

            virtual ~TupleBase() {}
        };
    }

    template <typename... Types>
    class Tuple : public tuple_base::TupleBase<typename IndexSetGenerator<sizeof... (Types)>::Type, Types...> {

        typedef tuple_base::TupleBase<typename IndexSetGenerator<sizeof... (Types)>::Type, Types...> BaseType;

    public:

        typedef Tuple<Types...> Type;

        JUTIL_GENERATE_METHODS(Tuple, default)

        template <
            typename... CTypes,
            typename = typename jutil::Enable<(sizeof... (Types) == sizeof... (CTypes))>::Type
        >
        explicit constexpr Tuple(CTypes&&... values) noexcept : BaseType(jutil::rval<CTypes>(values)...) {}

        constexpr size_t size() const noexcept {return sizeof...(Types);}

        virtual ~Tuple() {}
    };

    template <size_t i, typename ...>
    struct GetType;

    template<size_t i, typename... N>
    struct GetType<i, Tuple<N...> > {
        using Type = typename TypeAtIndex<i, N...>::Type;
    };

    template <size_t index, typename... Types>
    const IndexedType<index, Types...> &get(const Tuple<Types...> &t) noexcept {
        const tuple_base::TupleElement<index, IndexedType<index, Types...> > &ele = t;
        return ele.Value;
    }

    template <size_t index, typename... Types>
    IndexedType<index, Types...> &get(Tuple<Types...> &t) noexcept {
        tuple_base::TupleElement<index, IndexedType<index, Types...> > &ele = t;
        return ele.Value;
    }

    template <size_t index, typename... Types>
    typename RemoveReference<IndexedType<index, Types...> >::Type &get(Tuple<Types...> &&t) noexcept {
        tuple_base::TupleElement<index, typename RemoveReference<IndexedType<index, Types...> >::Type> &ele = t;
        return ele.Value;
    }

    namespace {
        template <typename T, typename F, size_t... n>
        inline void _forEach(T &t, F f, IndexSet<n...>) {
            JUTIL_FUNC_EXPAND(f, get<n>(t), n);
        }
    }

    template <typename... N, typename F>
    inline void forEach(Tuple<N...> &t, F f) {
        _forEach(t, f, IndexSetGenerator<sizeof...(N)>());
    }
}

#endif // JUTIL_TUPLE_H
