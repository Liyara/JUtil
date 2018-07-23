#include "Core/Thread.h"
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


    Thread::Thread () : shoudlPause(false), pauseLength(-1), cond(PTHREAD_COND_INITIALIZER), thread(0) {
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
        pauseLength = l;
        shoudlPause = true;
    }

    void Thread::tellUnpause() {
        shoudlPause = false;
        pthread_cond_signal(&cond);
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

    void Thread::pause(long l) {
        if (l > 0) {
            #ifdef _WIN32
                Sleep(l);
            #elif defined __linux__
                usleep(l);
            #endif // _WIN32
        } else {
            pthread_mutex_lock(&mut);
            pthread_cond_wait(&pcond, &mut);
            pthread_mutex_unlock(&mut);
        }
    }

    void Thread::unpause() {
        pthread_mutex_lock(&mut);
        pthread_cond_broadcast(&pcond);
        pthread_mutex_unlock(&mut);
    }

    void *Thread::connector(void *object) {
        Thread *obj = static_cast<Thread*>(object);
        obj->main();
        pthread_exit(NULL);
        return NULL;
    }

    void Thread::yield() {
        if (pauseLength <= 0) {
            pthread_mutex_lock(&mut);
            while (shoudlPause) {
                pthread_cond_wait(&cond, &mut);
            }
            pthread_mutex_unlock(&mut);
        } else {
            #ifdef _WIN32
                Sleep(pauseLength);
            #elif defined __linux__
                usleep(pauseLength);
            #endif // _WIN32
            pauseLength = -1;
            shoudlPause = false;
        }
    }
}

    /*pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    //jutil::Queue<pthread_cond_t> conditions;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    size_t tCount = 0, stock = 0;

    Thread::Thread(void (*cb)(Thread*)) : callback(cb), data(this, callback), id(stock) {
        pthread_mutex_lock(&lock);
        ++tCount;
        ++stock;
        //conditions.insert(PTHREAD_COND_INITIALIZER);
        //condition = conditions.end() - 1;
        pthread_mutex_unlock(&lock);
    }

    void Thread::pause() {
        locked(true);
        pthread_cond_wait(&cond, &lock);
        locked(false);
    }

    void Thread::resume() {
        locked(true);
        pthread_cond_signal(&cond);
        locked(false);
    }

    void Thread::run() {
        pthread_create(&thread, NULL, Thread::connect, static_cast<void*>(&data));
    }

    void Thread::wait(size_t time) JUTIL_CN_ {
        #ifdef _WIN32
            Sleep(time);
        #elif defined __linux__
            usleep(time);
        #endif // _WIN32
    }

    void Thread::join() JUTIL_CN_ {
        pthread_join(thread, NULL);
    }

    void Thread::locked(bool state) JUTIL_CN_ {
        if (state) {
            pthread_mutex_lock(&lock);
        } else {
            pthread_mutex_unlock(&lock);
        }
    }

    void *Thread::connect(void *arg) {
        CallbackData *dat = static_cast<CallbackData*>(arg);
        (jutil::get<1>(*dat))(jutil::get<0>(*dat));
        pthread_exit(NULL);
        return NULL;
    }

    size_t Thread::getID() JUTIL_CN_ {
        return id;
    }

    Thread::~Thread() {
        pthread_mutex_lock(&lock);
        --tCount;
        pthread_mutex_unlock(&lock);
    }*/
