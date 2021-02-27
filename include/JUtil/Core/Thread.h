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

#define JUTIL_THREAD_IMPL \
    jutil::Queue<jutil::Thread*> runningThreads; \
    jutil::Mutex jutil::globalMutex = jutil::createMutexObject();

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

    extern Queue<Thread*> runningThreads;
    extern Mutex globalMutex;

    inline Mutex createMutexObject() {
        #ifdef JUTIL_WINDOWS
            return CreateMutex(NULL, FALSE, NULL);
        #elif defined JUTIL_LINUX
            pthread_mutex_t *m = new pthread_mutex_t;;
            pthread_mutex_init(m, NULL);
            return (Mutex)m;
        #endif
    }

    inline bool mutexLock(Mutex *const mut) {
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

    inline bool mutexUnlock(Mutex *const mut) {
        #ifdef JUTIL_WINDOWS
            return ReleaseMutex(*mut);
        #elif defined JUTIL_LINUX
            return !pthread_mutex_unlock(const_cast<pthread_mutex_t*>(static_cast<pthread_mutex_t *const>(*mut)));
        #endif
    }

    class JUTIL_PUBLIC_ ThreadGroup JUTIL_NONCOPYABLE {
    public:
        ThreadGroup();

        bool addMember(Thread*);

        bool revokeMember(Thread*);

        bool isMember(Thread*);

        jutil::Queue<Thread*> getMembers();

        Mutex * const getMutex();

    private:
        Queue<Thread*> members;
        Mutex mutex;
    };

    class JUTIL_PUBLIC_ Thread JUTIL_NONCOPYABLE {
    public:

        Thread(ThreadGroup *g) : handle(0), id(0), group(g), shouldPause(false), paused(false), waiting(false), shouldStop(false), pauseLength(-1), mutex(createMutexObject()) {}

        bool start() {
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

        static ThreadID getCurrentThreadID() {
            #ifdef JUTIL_WINDOWS
                return GetCurrentThreadId();
            #elif defined JUTIL_LINUX
                return pthread_self();
            #endif
        }

        static Thread *getCurrentThread() {
            ThreadID id = getCurrentThreadID();
            for (auto &i: runningThreads) if (i->id == id) return i;
            return JUTIL_NULLPTR;
        }

        ThreadGroup *const getGroup() {
            return group;
        }

        bool setGroup(ThreadGroup *g) {
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

        bool join() {
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

        bool tellPause(long l) {
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

        bool tellUnpause() {
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

        bool tellStop() {
            if (!shouldStop) {
                shouldStop = true;
                return true;
            } else return false;
        }

        static bool requestGroupWait() {
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

        bool running() {
            return !runningThreads.empty() && runningThreads.find(this);
        }

        static bool requestGroupResume() {
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

        bool pausedOrWaiting() const {
            return paused || waiting;
        }

        ~Thread() {
            if (running()) {
                shouldStop = true;
                yield();
            }
        }

    private:
        ThreadHandle handle;
        ThreadID id;
        ThreadGroup *group;
        
        #ifdef JUTIL_WINDOWS
            static DWORD WINAPI connector(LPVOID object) {
        #elif defined JUTIL_LINUX
            static void *connector(void *object) {
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

        bool shouldPause, paused, waiting, shouldStop;
        long pauseLength;

        Mutex mutex;

    protected:

        static void yield() {
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

        virtual void onPause() = 0;
        virtual void onUnpause() = 0;
        virtual void onStop() = 0;

        virtual void main() = 0;
    };
        
        
    inline ThreadGroup::ThreadGroup() : mutex(createMutexObject()) {}

    inline bool ThreadGroup::addMember(Thread *t) {
        if (!(t->getGroup()) && (members.empty() || !members.find(t))) {
            members.insert(t);
            t->setGroup(this);
            return true;
        } else return false;
    }

    inline bool ThreadGroup::revokeMember(Thread *t) {
        size_t index;
        if (!t->getGroup() || t->getGroup() != this || members.empty() || !members.find(t, &index)) {
            return false;
        } else {
            t->setGroup(JUTIL_NULLPTR);
            members.erase(index);
            return true;
        }
    }

    inline bool ThreadGroup::isMember(Thread *t) {
        return (!members.empty() && members.find(t));
    }

    inline jutil::Queue<Thread*> ThreadGroup::getMembers() {
        return members;
    }

    inline Mutex * const ThreadGroup::getMutex() {
        return &mutex;
    }


    
}

#endif // JUTIL_THREAD_H
