#ifndef JUTIL_TIMER_H
#define JUTIL_TIMER_H

#include "JUtil/Core/Error.h"

#define TIMER_ERR_INDEX   0x0E

#ifdef JUTIL_ERR

    #define TIMERERR JUTIL_ERR_TIMER

    #define TIMERERR_OUTER              ((CORE_ERR_INDEX << 4) | TIMER_ERR_INDEX)
    #define TIMERERR_INNER_ORDER        0x0000
    #define TIMERERR_INNER_TYPE         0x0001
    #define TIMERERR_ORDER_MSG          "Timer order problem!"
    #define TIMERERR_TYPE_MSG           "Wrong type!"
    #define TIMERERR_ORDER_INVOKE       JUTIL_INVOKE_ERR(TIMERERR_OUTER, TIMERERR_INNER_ORDER, JUTIL_ERR_MSG(TIMERERR_ORDER_MSG))
    #define TIMERERR_TYPE_INVOKE        JUTIL_INVOKE_ERR(TIMERERR_OUTER, TIMERERR_INNER_TYPE, JUTIL_ERR_MSG(TIMERERR_TYPE_MSG))

#else
    #define TIMERERR_ORDER_INVOKE
    #define TIMERERR_TYPE_INVOKE
#endif

namespace jutil JUTIL_PUBLIC_ {

    long long queryGlobalTimer();

    enum {
        NANOSECONDS,
        MICROSECONDS,
        MILLISECONDS,
        SECONDS
    };

    class JUTIL_PUBLIC_ Timer {
    public:

        Timer();

        Timer(const Timer&);
        Timer &operator=(const Timer&);

        #ifdef JUTIL_CPP11
            Timer(Timer&&);
            Timer &operator=(Timer&&);
        #endif

        void start();

        long double get(unsigned = SECONDS);

        long double fastForward(long double, unsigned = SECONDS);

        long double rewind(long double, unsigned = SECONDS);

        long double stop(unsigned = SECONDS);

        void set(long double, unsigned = SECONDS);

        long double restart(unsigned = SECONDS);

    private:
        long long ct;
        long double freq;
    };
}

#endif // JUTIL_TIMER_H
