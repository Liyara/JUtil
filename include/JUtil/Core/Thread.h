#ifndef JUTIL_THREAD_H
#define JUTIL_THREAD_H

#include "JUtil/Core/NonCopyable.h"
#include "JUtil/Core/integers.h"
#include <JUtil/Core/Tuple.hpp>
#include <JUtil/Container/Queue.hpp>

#include "JUtil/Container/Map.hpp"
#include "JUtil/IO/IO.h"
#include <stdio.h>
#include <stdlib.h>
#include "JUtil/Core/Timer.h"

#ifdef JUTIL_LINUX
    #include <unistd.h>
#endif // JUTIL_LINUX

#ifdef JUTIL_WINDOWS
     #include "windows.h"
#elif defined JUTIL_LINUX
    #include <pthread.h>    // POSIX Threads
#endif // _WIN32

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

    jutil::Queue<Thread*> runningThreads;

    Mutex createMutexObject() {
        #ifdef JUTIL_WINDOWS
            return CreateMutex(NULL, FALSE, NULL);
        #elif defined JUTIL_LINUX
            pthread_mutex_t *m = new pthread_mutex_t;;
            pthread_mutex_init(m, NULL);
            return (Mutex)m;
        #endif
    }

    Mutex globalMutex = createMutexObject();

    ThreadGroup::ThreadGroup() : mutex(createMutexObject()) {}

    bool ThreadGroup::addMember(Thread *t) {
        if (!(t->getGroup()) && (members.empty() || !members.find(t))) {
            members.insert(t);
            t->setGroup(this);
            return true;
        } else return false;
    }

    bool ThreadGroup::revokeMember(Thread *t) {
        size_t index;
        if (!t->getGroup() || t->getGroup() != this || members.empty() || !members.find(t, &index)) {
            return false;
        } else {
            t->setGroup(JUTIL_NULLPTR);
            members.erase(index);
            return true;
        }
    }

    bool ThreadGroup::isMember(Thread *t) {
        return (!members.empty() && members.find(t));
    }

    jutil::Queue<Thread*> ThreadGroup::getMembers() {
        return members;
    }

    Mutex * const ThreadGroup::getMutex() {
        return &mutex;
    }

    bool mutexLock(Mutex *const mut) {
        #ifdef JUTIL_WINDOWS
            DWORD result;
            while (true) {
                result = WaitForSingleObject(*mut, INFINITE);
                return (result == WAIT_OBJECT_0);
            }
        #elif defined JUTIL_LINUX
            bool r;
            r = !pthread_mutex_lock(const_cast<pthread_mutex_t*>(static_cast<pthread_mutex_t *const>(*mut)));
            return r;
        #endif
    }

    bool mutexUnlock(Mutex *const mut) {
        #ifdef JUTIL_WINDOWS
            return ReleaseMutex(*mut);
        #elif defined JUTIL_LINUX
            return !pthread_mutex_unlock(const_cast<pthread_mutex_t*>(static_cast<pthread_mutex_t *const>(*mut)));
        #endif
    }

    Thread::Thread(ThreadGroup *g) : handle(0), id(0), group(g), shouldPause(false), paused(false), waiting(false), shouldStop(false), pauseLength(-1), mutex(createMutexObject()) {}

    bool Thread::start() {
        bool r;
        mutexLock(&mutex);
        if (!running()) {
            #ifdef JUTIL_WINDOWS
                handle = CreateThread(NULL, 0, Thread::connector, static_cast<void*>(this), 0, &id);
                r = handle;
            #elif defined JUTIL_LINUX
                r =  (!(pthread_create(&handle, NULL, Thread::connector, static_cast<void*>(this))));
            #endif
            if (r) {
                mutexLock(&globalMutex);
                runningThreads.insert(this);
                mutexUnlock(&globalMutex);
            }
        } else r = false;
        mutexUnlock(&mutex);
        return r;
    }

    #ifdef JUTIL_WINDOWS
        DWORD WINAPI Thread::connector(LPVOID object) {
    #elif defined JUTIL_LINUX
        void *Thread::connector(void *object) {
    #endif
        Thread *obj = static_cast<Thread*>(object);
        #ifdef JUTIL_LINUX
            obj->id = pthread_self();
        #endif
        obj->main();
        obj->shouldStop = true;
        yield();
        #ifdef JUTIL_WINDOWS
            return 0;
        #elif defined JUTIL_LINUX
            return NULL;
        #endif
    }

    ThreadID Thread::getCurrentThreadID() {
        #ifdef JUTIL_WINDOWS
            return GetCurrentThreadId();
        #elif defined JUTIL_LINUX
            return pthread_self();
        #endif
    }

    Thread *Thread::getCurrentThread() {
        ThreadID id = getCurrentThreadID();
        for (auto &i: runningThreads) if (i->id == id) return i;
        return JUTIL_NULLPTR;
    }

    ThreadGroup *const Thread::getGroup() {
        return group;
    }

    bool Thread::setGroup(ThreadGroup *g) {
        bool r;
        mutexLock(&mutex);
        if (!group && g) {
            group = g;
            r = true;
        } else if (group && !g) {
            group = JUTIL_NULLPTR;
            r = true;
        } else r = false;
        mutexUnlock(&mutex);
        return r;
    }

    bool Thread::join() {
        if (getCurrentThreadID() == id) return false;
        else {
            #ifdef JUTIL_WINDOWS
                WaitForSingleObject(handle, INFINITE);
            #elif defined JUTIL_LINUX
                pthread_join(handle, NULL);
            #endif
            return true;
        }
    }

    bool Thread::tellPause(long l) {
        bool r;
        mutexLock(&mutex);
        if (!shouldPause) {
            pauseLength = l;
            shouldPause = true;
            r = true;
        } else r = false;
        mutexUnlock(&mutex);
        return r;
    }

    bool Thread::tellUnpause() {
        bool r;
        mutexLock(&mutex);
        if (shouldPause) {
            shouldPause = false;
            pauseLength = -1;
            r = true;
        } else r = false;
        mutexUnlock(&mutex);
        return r;
    }

    bool Thread::tellStop() {
        if (!shouldStop) {
            shouldStop = true;
            return true;
        } else return false;
    }

    void Thread::yield() {
        Thread *thread = getCurrentThread();
        if (thread) {
            if (thread->shouldStop) {
                thread->onStop();
                #ifdef JUTIL_WINDOWS
                    ExitThread(0);
                #elif defined JUTIL_LINUX
                    pthread_exit(NULL);
                #endif
                thread->handle = 0;
                thread->id = 0;
                size_t i;
                mutexLock(&globalMutex);
                if (!runningThreads.empty() && runningThreads.find(thread, &i)) runningThreads.erase(i);
                mutexUnlock(&globalMutex);
            } else if (thread->shouldPause) {
                thread->onPause();
                Timer timer;
                timer.start();
                while (thread->shouldPause && (thread->pauseLength <= 0 || timer.get(MILLISECONDS) < thread->pauseLength)) thread->paused = true;
                thread->paused = false;
                thread->onUnpause();
            }
        }
    }

    bool Thread::requestGroupWait() {
        Thread *requestingThread = getCurrentThread();
        if (requestingThread) {
            ThreadGroup *requestedGroup = requestingThread->group;
            if (requestedGroup) {
                return mutexLock(requestedGroup->getMutex());
            } else {
                return mutexLock(&globalMutex);
            }
        } else {
            return mutexLock(&globalMutex);
        }
    }

    bool Thread::running() {
        return !runningThreads.empty() && runningThreads.find(this);
    }

    bool Thread::requestGroupResume() {
        Thread *requestingThread = getCurrentThread();
        if (requestingThread) {
            ThreadGroup *requestedGroup = requestingThread->group;
            if (requestedGroup) {
                return mutexUnlock(requestedGroup->getMutex());
            } else {
                return mutexUnlock(&globalMutex);
            }
        } else {
            return mutexUnlock(&globalMutex);
        }
    }

    bool Thread::pausedOrWaiting() const {
        return paused || waiting;
    }

    Thread::~Thread() {
        if (running()) {
            shouldStop = true;
            yield();
        }
    }
}

#endif // JUTIL_THREAD_H
