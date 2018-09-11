#ifndef JUTIL_THREAD_H
#define JUTIL_THREAD_H

#include "Core/Macro.h"

#ifdef JUTIL_CPP11

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

        bool isPaused() const;

    private:
        uintptr_t thread;

        static void *connector(void*);

        void *cond;
        static void *pcond, *mut;
        bool shoudlPause, paused;
        long pauseLength;
    protected:
        virtual void main() = 0;
        void yield();
        static void pause(unsigned long = 0);

        static void sync(Thread*);

    };
}

#endif

#endif // JUTIL_THREAD_H
