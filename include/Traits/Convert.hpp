#ifndef JUTIL_CONVERT_H
#define JUTIL_CONVERT_H

#include "declval.hpp"
#include "Validator.hpp"

namespace jutil {
    template <typename T, typename T2>
    class JUTIL_PUBLIC_ Convert final {

        template <typename C, typename D>
        struct JUTIL_PRIVATE_ Confirm final {};

        template <typename C, typename D>
        static constexpr Valid test(
            Confirm<
                typename RemoveReference<C>::Type, decltype(
                    static_cast<typename RemoveReference<D>::Type>(
                        declval<typename RemoveReference<C>::Type>()
                    )
                )
            >*
        ) noexcept;

        template <typename, typename>
        static constexpr Invalid test(...) noexcept;

    public:
        enum {Value = (sizeof(test<T, T2>(0)) == sizeof(Valid))};
    };
}
#endif // JUTIL_CONVERT_H
