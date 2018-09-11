#ifndef JUTIL_TYPE_MANIPULATORS_H
#define JUTIL_TYPE_MANIPULATORS_H

#if __cplusplus >= 201103L

namespace jutil {
    template <typename T>
    struct RemoveReference      {typedef T Type;};
    template <typename T>
    struct RemoveReference<T&>  {typedef T Type;};
    template <typename T>
    struct RemoveReference<T&&> {typedef T Type;};
    template <typename T>
    struct AddReference         {typedef T& Type;};
    template <typename T>
    struct AddReference<T&>     {typedef T& Type;};
    template <typename T>
    struct AddReference<T&&>    {typedef T& Type;};
    template <typename T>
    struct AddRvalue            {typedef T&& Type;};
    template <typename T>
    struct AddRvalue<T&>        {typedef T&& Type;};
    template <typename T>
    struct AddRvalue<T&&>       {typedef T&& Type;};
    template <typename T>
    struct RemovePointer        {typedef T Type;};
    template <typename T>
    struct RemovePointer<T*>    {typedef T Type;};
    template <typename T>
    struct AddPointer           {typedef T* Type;};
    template <typename T>
    struct AddPointer<T*>       {typedef T* Type;};
    template <typename T>
    struct RemoveConst          {typedef T Type;};
    template <typename T>
    struct RemoveConst<const T> {typedef T Type;};
    template <typename T>
    struct AddConst             {typedef const T Type;};
    template <typename T>
    struct AddConst<const T>    {typedef const T Type;};


    template <typename T>
    T &&rval
    (typename RemoveReference<T>::Type &arg) noexcept
    {return static_cast<T&&>(arg);}
    template <typename T>
    T &&rval
    (typename RemoveReference<T>::Type &&arg) noexcept
    {return static_cast<T&&>(arg);}
    template <typename T>
    constexpr typename AddReference<T>::Type lval
    (typename RemoveReference<T>::Type &arg) noexcept
    {return static_cast<T&>(arg);}
    template <typename T>
    constexpr typename AddReference<T>::Type lval
    (typename RemoveReference<T>::Type &&arg) noexcept
    {return static_cast<T&>(arg);}
    template <typename T>
    constexpr typename RemoveReference<T>::Type &&move
    (T &&arg) noexcept
    {return static_cast<typename RemoveReference<T>::Type&&>(arg);}

    //Variadic Types
    template <typename T, T... n>
    struct Group {
        typedef T Type;
        static constexpr T Value[sizeof...(n)] = {n...};
    };

    template <typename T, T... n>
    constexpr T Group<T, n...>::Value[sizeof...(n)];

    template<typename, typename>
    struct concat;

    template<typename T, T... n1, T... n2>
    struct concat<Group<T, n1...>, Group<T, n2...>> {
        using Type = Group<T, n1..., n2...>;
    };

    template <typename T, T n_0, T... n>
    struct RemoveFirst {
        using Type = Group<T, n...>;
    };

    template<typename T, T n>
    struct RemoveFirst<T, n> {
        using Type = Group<T>;
    };

    template <typename T, T...>
    struct RemoveLast;

    template <typename T, T n1_0, T... n1>
    struct RemoveLast<T, n1_0, n1...> {
        using Type = typename concat<Group<T, n1_0>, typename RemoveLast<T, n1...>::Type>::Type;
    };

    template <typename T, T n>
    struct RemoveLast<T, n> {
        using Type = Group<T>;
    };

    template<typename T, size_t i, T... n>
    struct AtIndex;

    template<typename T, size_t i, T n_0, T... n>
    struct AtIndex<T, i, n_0, n...> {
        static constexpr T Value = AtIndex<T, i - 1, n...>::Value;
    };

    template <typename T>
    struct AtIndex<T, 0> {

    };

    template<typename T, size_t i, T n_0, T... n>
    constexpr T AtIndex<T, i, n_0, n...>::Value;

    template<typename T, T n_0, T... n>
    struct AtIndex<T, 0, n_0, n...> {
        static constexpr T Value = n_0;
    };

    template<typename T, T n_0, T... n>
    constexpr T AtIndex<T, 0, n_0, n...>::Value;

    template<size_t, typename...>
    struct TypeAtIndex;

    template<size_t i, typename N_0, typename... N>
    struct TypeAtIndex<i, N_0, N...> {
        using Type = typename TypeAtIndex<i - 1, N...>::Type;
    };

    template <>
    struct TypeAtIndex<0> {
        using Type = void;
    };

    template<typename N_0, typename... N>
    struct TypeAtIndex<0, N_0, N...> {
        using Type = N_0;
    };

    template <size_t... n>
    struct IndexSet {
        typedef IndexSet<n...> Type;
    };

    template <size_t i, typename>
    struct AppendSet;

    template <size_t i, size_t... n>
    struct AppendSet<i, IndexSet<n...> > : IndexSet<n..., i> {};

    template <size_t c>
    struct IndexSetGenerator : AppendSet<c - 1, typename IndexSetGenerator<c - 1>::Type>::Type {};

    template<>
    struct IndexSetGenerator<1> : IndexSet<0> {};
}

#endif

#endif // JUTIL_TYPE_MANIPULATORS_H
