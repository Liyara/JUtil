#ifndef JUTIL_PAIR_H
#define JUTIL_PAIR_H

#ifdef JUTIL_CPP11

#define PAIR_ERR_INDEX   0x0D

namespace jutil  {
    template <typename A, typename B>
    class Pair : public Tuple<A, B> {
    public:

        typedef A TypeA;
        typedef B TypeB;
        typedef Pair<A, B> Type;

        JUTIL_GENERATE_METHODS(Pair, default)

        template<
            typename C,
            typename D,
            typename = typename Enable<Convert<C, A>::Value>::Type,
            typename = typename Enable<Convert<D, B>::Value>::Type
        >
        constexpr Pair(C &&c, D &&d) : Tuple<A, B>(rval<C>(c), rval<D>(d)) {}

        A &first() noexcept {return get<0>(*this);}
        B &second() noexcept {return get<1>(*this);}

        Pair<B, A> &&reverse() const noexcept {
            return Pair<B, A>(second(), first());
        }

        Type &clear() noexcept {
            first() = A {};
            second() = B {};
            return *this;
        }

        bool operator==(const Pair<A, B> &p) const {
            return (get<0>(*this) == get<0>(p) && get<1>(*this) == get<1>(p));
        }

        bool operator!=(const Pair<A, B> &p) const {
            return !((*this) == p);
        }

        virtual ~Pair() {}
    };
}

#endif

#endif // JUTIL_PAIR_H
