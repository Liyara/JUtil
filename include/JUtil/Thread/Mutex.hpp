#ifndef _JUTIL_MUTEX_HPP_
#define _JUTIL_MUTEX_HPP_

#include <JUtil/Thread/thread_decl.h>

namespace jutil  {
    class Mutex {
        public:
            Mutex() {
                #ifdef JUTIL_WINDOWS
                    InitializeCriticalSection(&_handle);
                #elif defined JUTIL_LINUX
                    pthread_mutex_init(&_handle, NULL);
                #endif
            }

            Mutex(const Mutex& other) {
                _handle = other._handle;
            }

            Mutex(Mutex&& other) {
                _handle = move(other._handle);
            }

            Mutex& operator=(const Mutex& other) {
                _handle = other._handle;
                return *this;
            }

            Mutex& operator=(Mutex&& other) {
                _handle = move(other._handle);
                return *this;
            }

            virtual ~Mutex() {
                #ifdef JUTIL_LINUX
                    pthread_mutex_destroy(&_handle);
                #endif
            }

            bool lock() {
                #ifdef JUTIL_WINDOWS
                    DWORD result;
                    while (true) {
                        result = WaitForSingleObject(&_handle, INFINITE);
                        return (result == WAIT_OBJECT_0);
                    }
                #elif defined JUTIL_LINUX
                    bool r;
                    r = !pthread_mutex_lock(&_handle);
                    return r;
                #endif
            }

            bool unlock() {
                #ifdef JUTIL_WINDOWS
                    return ReleaseMutex(&_handle);
                #elif defined JUTIL_LINUX
                    return !pthread_mutex_unlock(&_handle);
                #endif
            }

        private:
            MutexHandle _handle;
            friend class Condition;
            friend class Thread;
    };
}

#endif // _JUTIL_MUTEX_HPP_