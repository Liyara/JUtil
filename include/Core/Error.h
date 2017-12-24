#ifndef JUTIL_ERROR_H
#define JUTIL_ERROR_H

#include "Macro.h"
#include "size_t.h"

#define JUTIL_ERR
#define JUTIL_INVOKE_ERR(a, b, c)   jutil::Error::write(a, b, c).handle();
#define JUTIL_ERR_MSG(a)            a, __PRETTY_FUNCTION__
#define JUTIL_INVOKE_ERR_FATAL(a, b, c) jutil::FatalError::write(a, b, c);

#define ERR_OUTER                   0x0000
#define ERR_INNER                   0x0000
#define ERR_MSG                     "Error reporting failed!"
#define ERR_INVOKE                  JUTIL_INVOKE_ERR(ERR_OUTER, ERR_INNER, JUTIL_ERR_MSG(ERR_MSG))

namespace jutil JUTIL_PUBLIC_ {
    class JUTIL_PUBLIC_ Error {
    public:
        static Error &write(int, int, const char*, const char*) JUTIL_N_;
        static Error &read(size_t) JUTIL_N_;
        #ifdef JUTIL_CPP11
            Error() = delete;
            Error(const Error&) = delete;
            Error(Error&&) = delete;
            Error &operator=(const Error&) = delete;
            Error &operator=(Error&&) = delete;
        #endif
        static size_t stackSize() JUTIL_N_;
        static bool has(Error*) JUTIL_N_;
        virtual void handle() JUTIL_N_;
        static void handleStack() JUTIL_N_;
        static void destroyStack() JUTIL_N_;
        static void handleTop() JUTIL_N_;
        static void handleBottom() JUTIL_N_;
        Error &silence(bool) JUTIL_N_;
        virtual ~Error() JUTIL_N_;
    protected:
        int outer, inner;
        const char *msg, *func;
        bool written;
        JUTIL_CX_ Error(int, int, const char*, const char*) JUTIL_N_;
    private:
        #ifndef JUTIL_CPP11
            Error();
            Error(const Error&);
            Error &operator=(const Error&);
        #endif
        void destroySelf() JUTIL_N_;
        bool silenced;
    };
    class JUTIL_PUBLIC_ FatalError : private Error {
    public:
        static void write(int, int, const char*, const char*) JUTIL_N_;
    private:
        void handle() JUTIL_NO_;
        JUTIL_CX_ FatalError(int, int, const char*, const char*) JUTIL_N_;
    };
}

#endif // JUTIL_ERROR_H
