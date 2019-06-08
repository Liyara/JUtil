#ifndef JUTIL_THREAD_H
#define JUTIL_THREAD_H

#include "JUtil/Core/NonCopyable.h"
#include "JUtil/Core/integers.h"
#include <JUtil/Core/Tuple.hpp>
#include <JUtil/Container/Queue.hpp>

#define THREAD_ERR_INDEX   0x0C

#define THREAD_EXEC(a) yield(); (a)
#define THREAD_PAUSE(n) tellPause(n); yield()

namespace jutil JUTIL_PUBLIC_ {

    #ifdef JUTIL_WINDOWS
        typedef void*           ThreadHandle;
        typedef void*           Mutex;
        typedef unsigned long   ThreadID;
    #elif defined JUTIL_LINUX
        typedef unsigned long   ThreadHandle;
        typedef unsigned long   ThreadID;
        typedef void*           Mutex;
    #endif

    class Thread;

    class JUTIL_PUBLIC_ ThreadGroup JUTIL_NONCOPYABLE {
    public:
        ThreadGroup();

        bool addMember(Thread*);
        bool revokeMember(Thread*);
        bool isMember(Thread*);

        jutil::Queue<Thread*> getMembers();

        Mutex* const getMutex();

    private:
        Queue<Thread*> members;
        Mutex mutex;
    };

    class JUTIL_PUBLIC_ Thread JUTIL_NONCOPYABLE {
    public:

        Thread(ThreadGroup* = JUTIL_NULLPTR);

        bool start();
        bool join();

        bool tellPause(long = -1);
        bool tellUnpause();
        bool tellStop();

        ThreadID getID() const;

        bool pausedOrWaiting() const;

        static ThreadID getCurrentThreadID();
        static Thread *getCurrentThread();

        ThreadGroup *const getGroup();
        bool setGroup(ThreadGroup*);

        bool running();

        static bool requestGroupWait();
        static bool requestGroupResume();

        virtual ~Thread();

    private:
        ThreadHandle handle;
        ThreadID id;
        ThreadGroup *group;

        #ifdef JUTIL_WINDOWS
            static unsigned long __stdcall connector(void*);
        #elif defined JUTIL_LINUX
            static void *connector(void*);
        #endif

        bool shouldPause, paused, waiting, shouldStop;
        long pauseLength;

        Mutex mutex;

    protected:

        static void yield();

        virtual void onPause() = 0;
        virtual void onUnpause() = 0;
        virtual void onStop() = 0;

        virtual void main() = 0;
    };
}

#endif // JUTIL_THREAD_H
