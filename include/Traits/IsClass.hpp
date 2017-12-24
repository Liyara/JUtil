#ifndef JUTIL_IS_CLASS_H
#define JUTIL_IS_CLASS_H

#include "Validator.hpp"

namespace jutil {

    template <typename T>
    class IsClass {

        template <typename T2>
        static constexpr Valid test(int T2::*);

        template <typename>
        static constexpr Invalid test(...);

    public:

        enum {Value = (sizeof(test<T>(0)) == sizeof(Valid))};

    };
}

#endif // JUTIL_IS_CLASS_H
