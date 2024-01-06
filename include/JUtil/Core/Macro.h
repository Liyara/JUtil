#ifndef JUTIL_MACRO_H
#define JUTIL_MACRO_H

/**
    @section DESCRIPTION

    This is a top-level header required by ALL parts of jutil.

    There are many macros and forward declaraction made here,
    but none of them are intended for use by users of jutil;
    they are for use only by jutil itself.

    As such, they will not be described here,
    altough you may you use them if you wish.
*/

#include "size_t.h"

#define MACRO_ERR_INDEX   0x02

#define JUTIL__

#define JUTIL_INTERNAL_  JUTIL__ __

namespace jutil  {

    #ifndef JUTIL_CONTAINER_H
        template <typename>
        class  Iterator;
        template <typename, typename, typename, typename>
        class  Container;
    #endif
    #ifndef JUTIL_CONTIGUOUS_CONTAINER_H
        template<typename, typename, typename>
        class  ContiguousContainer;
    #endif
    #ifndef JUTIL_NON_CONTIGUOUS_CONTAINER_H
        template <typename, typename, typename, typename>
        class  __NonContiguousContainerInternalIterator;
        template <typename, typename, typename, typename>
        struct  __NonContiguousContainerInternalNode;
    #endif
    #ifndef JUTIL_QUEUE_H
        template <typename, typename>
        class  Queue;
    #endif
    #ifndef JUTIL_MAP_H
        template<typename, typename>
        class  __MapInternalIterator;
        template <typename, typename>
        class  Map;
    #endif
    #ifndef JUTIL_PAIR_H
        template <typename, typename>
        class  Pair;
    #endif
    #ifdef JUTIL_CPP11
        #ifndef JUTIL_TUPLE_H
            template <typename...>
            class  Tuple;
        #endif
    #endif
    #ifndef JUTIL_NON_COPYABLE_H
        class  NonCopyable;
    #endif
    #ifndef JUTIL_STRING_H
        template <typename>
        class  StringBase;
        struct  StringInterface;
    #endif
    #ifndef JUTIL_ERROR_H
        class  Error;
    #endif
    #ifndef JUTIL_TIMER_H
        class  Timer;
    #endif
    #ifndef JUTIL_THREAD_H
        class  ThreadGroup;
        class  Thread;
    #endif
    #ifndef JUTIL_RNG_H
        class  RNG;
    #endif
    #ifndef JUTIL_IO_H
        namespace io_base  {
            template <typename>
            class  OutputHandler;
            template <typename>
            class  InputHandler;
        }
        class  File;
    #endif
    #ifndef JUTIL_TRAITS_H
        template <typename T, T>
        class  Validator;
        template <bool, typename = void>
        class  Enable;
        template <typename, typename>
        class  Convert;
    #endif
}

#define JUTIL_GET_ARG(a, b, c, d, e, f, g, h, i, N, ...) N
#define JUTIL_COUNT_ARGS_(...) JUTIL_GET_ARG(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define JUTIL_UNROLL_1(a)       a
#define JUTIL_UNROLL_2(a)       JUTIL_UNROLL_1(a) JUTIL_UNROLL_1(a)
#define JUTIL_UNROLL_3(a)       JUTIL_UNROLL_2(a) JUTIL_UNROLL_1(a)
#define JUTIL_UNROLL_4(a)       JUTIL_UNROLL_2(JUTIL_UNROLL_2(a))
#define JUTIL_UNROLL_5(a)       JUTIL_UNROLL_4(a) JUTIL_UNROLL_1(a)
#define JUTIL_UNROLL_6(a)       JUTIL_UNROLL_2(JUTIL_UNROLL_3(a))
#define JUTIL_UNROLL_7(a)       JUTIL_UNROLL_6(a) JUTIL_UNROLL_1(a)
#define JUTIL_UNROLL_8(a)       JUTIL_UNROLL_2(JUTIL_UNROLL_4(a))
#define JUTIL_UNROLL_9(a)       JUTIL_UNROLL_8(a) JUTIL_UNROLL_1(a)
#define JUTIL_UNROLL___(N, a)   JUTIL_UNROLL_##N(a)
#define JUTIL_UNROLL__(N, a)    JUTIL_UNROLL___(N, a)
#define JUTIL_UNROLL_(N, a)     JUTIL_UNROLL__(N, a)
#define JUTIL_CALL_UNROLL_1(func, ...)                          func(__VA_ARGS__)
#define JUTIL_CALL_UNROLL_2(func, a, b)                         JUTIL_CALL_UNROLL_1(func, a) JUTIL_CALL_UNROLL_1(func, b)
#define JUTIL_CALL_UNROLL_3(func, a, b, c)                      JUTIL_CALL_UNROLL_2(func, a, b) JUTIL_CALL_UNROLL_1(func, c)
#define JUTIL_CALL_UNROLL_4(func, a, b, c, d)                   JUTIL_CALL_UNROLL_2(func, a, b) JUTIL_CALL_UNROLL_2(func, c, d)
#define JUTIL_CALL_UNROLL_5(func, a, b, c, d, e)                JUTIL_CALL_UNROLL_4(func, a, b, c, d) JUTIL_CALL_UNROLL_1(func, e)
#define JUTIL_CALL_UNROLL_6(func, a, b, c, d, e, f)             JUTIL_CALL_UNROLL_3(func, a, b, c) JUTIL_CALL_UNROLL_3(func, d, e, f)
#define JUTIL_CALL_UNROLL_7(func, a, b, c, d, e, f, g)          JUTIL_CALL_UNROLL_6(func, a, b, c, d, e, f) JUTIL_CALL_UNROLL_1(func, g)
#define JUTIL_CALL_UNROLL_8(func, a, b, c, d, e, f, g, h)       JUTIL_CALL_UNROLL_4(func, a, b, c, d) JUTIL_CALL_UNROLL_4(func, e, f, g, h)
#define JUTIL_CALL_UNROLL_9(func, a, b, c, d, e, f, g, h, i)    JUTIL_CALL_UNROLL_8(func, a, b, c, d, e, f, g, h) JUTIL_CALL_UNROLL_1(i)
#define JUTIL_CALL_UNROLL___(N, func, ...)                      JUTIL_CALL_UNROLL_##N(func, __VA_ARGS__)
#define JUTIL_CALL_UNROLL__(N, func, ...)                       JUTIL_CALL_UNROLL___(N, func, __VA_ARGS__)
#define JUTIL_CALL_UNROLL_(N, func, ...)                        JUTIL_CALL_UNROLL__(N, func, __VA_ARGS__)
#define JUTIL_MAKE_FUNC(...)                                    __VA_ARGS__

#define JUTIL_COUNT_ARGS(...)           JUTIL_COUNT_ARGS_(__VA_ARGS__)
#define JUTIL_UNROLL(N, a)              JUTIL_UNROLL_(N, a)
#define JUTIL_CALL_UNROLL(func, ...)    JUTIL_CALL_UNROLL_(JUTIL_COUNT_ARGS(__VA_ARGS__), func, __VA_ARGS__)

#define JUTIL_C_        JUTIL__ const
#define JUTIL_D_        JUTIL__ virtual

#ifdef JUTIL_CPP11
    #define JUTIL_N_        JUTIL__ noexcept
    #define JUTIL_CN_       JUTIL__ JUTIL_C_ JUTIL_N_
    #define JUTIL_CX_       JUTIL__ constexpr
    #define JUTIL_EXPL_     JUTIL__ explicit
    #define JUTIL_EXPL_CX_  JUTIL__ JUTIL_EXPL_ JUTIL_CX_
    #define JUTIL_F_        JUTIL__ final
    #define JUTIL_OVERRIDE_ JUTIL__ override
    #define JUTIL_CO_       JUTIL__ JUTIL_C_ JUTIL_OVERRIDE_
    #define JUTIL_CNO_      JUTIL__ JUTIL_CN_ JUTIL_OVERRIDE_
    #define JUTIL_NO_       JUTIL__ JUTIL_N_ JUTIL_OVERRIDE_
    #define JUTIL_RVAL_     JUTIL__ &&
    #ifndef NULL
        #define NULL    nullptr
    #endif
    #define JUTIL_INIT(a, b)        a {b}
    #define JUTIL_INIT_DEFAULT(a)   a ()
    #define JUTIL_GENERATE_TEMPLATE_ALIASES_(T) \
        typedef T&&         Rvalue;\
        typedef const T&&   const_Rvalue;
    #define JUTIL_GENERATE_METHODS(T, func) T() = func;\
    T(const T&) = func;\
    T(T&&) = func;\
    T &operator=(const T&) = func;\
    T &operator=(T&&) = func;
    #define JUTIL_DEFAULT(T) T {}
#else
    #define JUTIL_N_        JUTIL__
    #define JUTIL_CN_       JUTIL__ JUTIL_C_
    #define JUTIL_CX_       JUTIL__
    #define JUTIL_EXPL_     JUTIL__
    #define JUTIL_F_        JUTIL__
    #define JUTIL_OVERRIDE_ JUTIL__
    #define JUTIL_CO_       JUTIL__ JUTIL_C_
    #define JUTIL_CNO_      JUTIL__ JUTIL_C_
    #define JUTIL_NO_       JUTIL__
    #define JUTIL_EXPL_CX_  JUTIL__ JUTIL_C_
    #define JUTIL_RVAL_     JUTIL__
    #ifndef NULL
        #define NULL    0
    #endif
    #define JUTIL_INIT(a, b)        a (b)
    #define JUTIL_INIT_DEFAULT(a)   a ()
    #define JUTIL_GENERATE_TEMPLATE_ALIASES_(T)
    #define JUTIL_GENERATE_METHODS(T, func) T() = func;\
    T(const T&) = func;\
    T &operator=(const T&) = func;
    #define JUTIL_DEFAULT(T) T()
#endif

#ifdef JUTIL_CPP17
    #define JUTIL_IFCX_ if constexpr
#else
#define JUTIL_IFCX_ if
#endif

#define JUTIL_GENERATE_TEMPLATE_ALIASES(T) JUTIL_GENERATE_TEMPLATE_ALIASES_(T)\
    typedef T           Type;\
    typedef T&          Reference;\
    typedef const T&    const_Reference;\
    typedef T*          Ptr;\
    typedef const T*    const_Ptr;

#define JUTIL_NULLPTR       NULL
#define JUTIL_NULLREF(a)    *((a*)JUTIL_NULLPTR)
#define JUTIL_NULL__(a)     JUTIL__ a = JUTIL_NULLPTR
#define JUTIL_NULL_(a)      JUTIL__ JUTIL_NULL__(a);

#define JUTIL_FREE_(a)      JUTIL__ {delete a; JUTIL_NULL_(a);};
#define JUTIL_FREE(a)       {if (a) {JUTIL_FREE_(a);}}

#define JUTIL_FOR_(a, b, c) for (JUTIL_INIT(auto (a), (b)); (a) < (c); ++(a))
#define JUTIL_FOR(a, b)     JUTIL_FOR_(a, 0, b)

#define JUTIL_FUNC_EXPAND(a, ...) char __jutil_expansion_array[] {(a(__VA_ARGS__), '\0')...}; (void) __jutil_expansion_array;

#define JUTIL_CAST_DERIVED        (static_cast<DerivedType*>(this))
#define JUTIL_CONST_CAST_DERIVED  (const_cast<DerivedType*>(static_cast<const DerivedType*>(this)))

#define JUTIL_FORWARD_TEMPLATE_1  template<typename>
#define JUTIL_FORWARD_TEMPLATE_2  template<typename, typename>
#define JUTIL_FORWARD_TEMPLATE_3  template<typename, typename, typename>
#define JUTIL_FORWARD_TEMPLATE_4  template<typename, typename, typename, typename>
#define JUTIL_FORWARD_TEMPLATE_5  template<typename, typename, typename, typename, typename>


//placement-new defined here to avoid including <new>
#ifndef JUTIL_DISABLE_NEW
    #ifdef JUTIL_CPP11
        #ifndef _NEW
        #define _NEW
            inline void* operator new(size_t, void* dat) JUTIL_N_ {
                return dat;
            }
            inline void* operator new[](size_t, void* dat) JUTIL_N_ {
                return dat;
            }
        #endif
        #define JUTIL_NEW(mem, val) new (mem) val
    #else
        #define JUTIL_NEW(mem, val) *(mem) = val
    #endif
#endif

#endif // JUTIL_MACRO_H
