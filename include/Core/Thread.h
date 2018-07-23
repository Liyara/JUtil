#ifndef JUTIL_THREAD_H
#define JUTIL_THREAD_H

#include "Core/Macro.h"
#include "Core/NonCopyable.h"
#include "Core/integers.h"
#include <Core/Tuple.hpp>

#define THREAD_EXEC(a) yield(); (a)
#define THREAD_PAUSE(n) tellPause(n); yield()

namespace jutil JUTIL_PUBLIC_ {

    class JUTIL_PUBLIC_ Thread : private jutil::NonCopyable {
    public:

        Thread ();
        virtual ~Thread();

        bool start();
        void join();

        void tellPause(long = -1);
        void tellUnpause();

        size_t getID() const;
        Thread *getThread(size_t);

        static void lock();
        static void unlock();

        static void pause(long = 0);
        static void unpause();
    private:
        uintptr_t thread;

        static void *connector(void*);

        void *cond;
        static void *pcond, *mut;
        bool shoudlPause;
        long pauseLength;
    protected:
        virtual void main() = 0;
        void yield();
    };
}

#endif // JUTIL_THREAD_H
