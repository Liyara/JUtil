#ifndef JUTIL_TYPE_MANIPULATORS_H
#define JUTIL_TYPE_MANIPULATORS_H

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
}
#endif // JUTIL_TYPE_MANIPULATORS_H
