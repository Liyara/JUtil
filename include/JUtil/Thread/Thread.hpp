#ifndef _JUTIL_THREAD_HPP_
#define _JUTIL_THREAD_HPP_

#include <JUtil/Thread/Mutex.hpp>
#include <JUtil/Thread/Condition.hpp>

namespace jutil  {

    class Thread {
        public:

            static void yield() {
                #ifdef JUTIL_WINDOWS
                    SwitchToThread();
                #elif defined JUTIL_LINUX
                    sched_yield();
                #endif
            }

            static void join(Thread *thread) {
                #ifdef JUTIL_WINDOWS
                    WaitForSingleObject(thread->_handle, INFINITE);
                #elif defined JUTIL_LINUX
                    pthread_join(thread->_handle, NULL);
                #endif
            }

            static ThreadID getThreadID() {
                #ifdef JUTIL_WINDOWS
                    return GetCurrentThreadId();
                #elif defined JUTIL_LINUX
                    return pthread_self();
                #endif
            }

            static Thread *getThread(ThreadID id) {
                Thread *thread = JUTIL_NULLPTR;
                threadManagerMutex().lock();
                for (Thread *t : runningThreads()) {
                    if (t->getID() == id) {
                        thread = t;
                        break;
                    }
                }
                threadManagerMutex().unlock();
                return thread;
            }

            static Thread *getCurrentThread() {
                return getThread(getThreadID());
            }

            static void sleep(unsigned long milliseconds) {
                #ifdef JUTIL_WINDOWS
                    Sleep(milliseconds);
                #elif defined JUTIL_LINUX
                    usleep(milliseconds * 1000);
                #endif
            }

            static long countProcessors() {
                #ifdef JUTIL_WINDOWS
                    SYSTEM_INFO sysInfo;
                    GetSystemInfo(&sysInfo);
                    return sysInfo.dwNumberOfProcessors;
                #elif defined JUTIL_LINUX
                    return sysconf(_SC_NPROCESSORS_ONLN);
                #endif
            }

            Thread() : _id(0) {}

            Thread(const Thread& other) {
                _handle = other._handle;
            }

            Thread(Thread&& other) {
                _handle = move(other._handle);
            }

            Thread& operator=(const Thread& other) {
                _handle = other._handle;
                return *this;
            }

            Thread& operator=(Thread&& other) {
                _handle = move(other._handle);
                return *this; 
            }

            ~Thread() {
                #ifdef JUTIL_WINDOWS
                    CloseHandle(_handle);
                #elif defined JUTIL_LINUX
                    pthread_join(_handle, NULL);
                #endif
            }

            void start() {
                #ifdef JUTIL_WINDOWS
                    _handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_run, (void*)this, 0, &_id);
                #elif defined JUTIL_LINUX
                    pthread_create(&_handle, NULL, (void*(*)(void*))_run, (void*)this);
                #endif
            }

            void stop() {
                _onStop();
                #ifdef JUTIL_WINDOWS
                    TerminateThread(_handle, 0);
                #elif defined JUTIL_LINUX
                    pthread_cancel(_handle);
                #endif
            }

            bool isRunning() {
                threadManagerMutex().lock();
                bool running = runningThreads().find(this);
                threadManagerMutex().unlock();
                return running;
            }

            ThreadID getID() const {
                return _id;
            }

            void join() {
                join(this);
            }

        private:
            ThreadHandle _handle;
            ThreadID _id;

            virtual void _onStart() {
                threadManagerMutex().lock();
                if (!runningThreads().find(this)) {
                    runningThreads().insert(this);
                    onStart();
                }
                threadManagerMutex().unlock();
            }

            virtual void _onStop() {
                threadManagerMutex().lock();
                if (runningThreads().find(this)) {
                    runningThreads().remove(this);
                    onStop();
                }
                threadManagerMutex().unlock();
            }

            static void _run(void *data) {
                Thread *thread = (Thread*)data;
                thread->_id = getThreadID();
                thread->_onStart();
                thread->run();
                thread->_onStop();
            }

            static Queue<Thread*> &runningThreads() {
                static Queue<Thread*> runningThreads = {};
                return runningThreads;
            }

        protected:

            virtual void run() = 0;
            virtual void onStart() {}
            virtual void onStop() {}
            static Mutex &threadManagerMutex() {
                static Mutex threadManagerMutex;
                return threadManagerMutex;
            }
    };
}

#endif // JUTIL_THREAD_H
