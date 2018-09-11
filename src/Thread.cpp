#include "Core/Thread.h"

#ifdef JUTIL_CPP11

#include "Container/Map.hpp"
#include "IO/IO.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
    #include "windows.h"
#elif defined __linux__
    #include <unistd.h>
#endif // _WIN32

namespace jutil {

    pthread_mutex_t Thread::mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t Thread::pcond = PTHREAD_COND_INITIALIZER;

    size_t cr = 0;
    jutil::Map<size_t, Thread*> threads;


    Thread::Thread () : shoudlPause(false), paused(false), pauseLength(-1), cond(PTHREAD_COND_INITIALIZER), thread(0) {
        lock();
        threads.insert(cr++, this);
        unlock();
    }

    Thread::~Thread() {
        lock();
        threads.erase(getID());
        unlock();
    }

    bool Thread::start() {
        return (!(pthread_create(&thread, NULL, Thread::connector, static_cast<void*>(this))));
    }

    void Thread::join() {
        pthread_join(thread, NULL);
    }

    void Thread::tellPause(long l) {
        lock();
        pauseLength = l;
        shoudlPause = true;
        unlock();
    }

    void Thread::tellUnpause() {
        lock();
        shoudlPause = false;
        pthread_cond_signal(&cond);
        unlock();
    }

    size_t Thread::getID() const {
        for (auto it = threads.begin(); it != threads.end(); ++it) {
            if (*it == this) {
                return &it;
            }
        }
        return -1;
    }

    Thread *Thread::getThread(size_t i) {
        return threads[i];
    }

    void Thread::lock() {
        pthread_mutex_lock(&mut);
    }

    void Thread::unlock() {
        pthread_mutex_unlock(&mut);
    }

    void Thread::pause(unsigned long l) {
        #ifdef JUTIL_WINDOWS
            Sleep(l);
        #elif defined JUTIL_LINUX
            usleep(l);
        #endif // _WIN32
    }

    void *Thread::connector(void *object) {
        Thread *obj = static_cast<Thread*>(object);
        obj->main();
        pthread_exit(NULL);
        return NULL;
    }

    void Thread::yield() {
        if (shoudlPause) {
            if (pauseLength <= 0) {
                pthread_mutex_lock(&mut);
                while (shoudlPause) {
                    paused = true;
                    pthread_cond_wait(&cond, &mut);
                }
                pthread_mutex_unlock(&mut);
            } else {
                paused = true;
                shoudlPause = false;
                #ifdef JUTIL_WINDOWS
                    Sleep(pauseLength);
                #elif defined JUTIL_LINUX
                    usleep(pauseLength);
                #endif // _WIN32
                pauseLength = -1;

            }
            paused = false;
        }
    }

    bool Thread::isPaused() const {
        return paused;
    }

    void Thread::sync(Thread *t) {
        t->tellPause();
        while (!t->isPaused()) {}
        t->tellUnpause();
        while (t->isPaused()) {}
    }
}

#endif
