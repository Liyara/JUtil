#ifndef _JUTIL_CONDITION_HPP_
#define _JUTIL_CONDITION_HPP_

#include <JUtil/Thread/thread_decl.h>

namespace jutil  {
    class Condition {
        public:
            Condition() {
                #ifdef JUTIL_WINDOWS
                    InitializeConditionVariable(&_handle);
                #elif defined JUTIL_LINUX
                    pthread_cond_init(&_handle, NULL);
                #endif
            }

            Condition(const Condition& other) {
                _handle = other._handle;
            }

            Condition(Condition&& other) {
                _handle = move(other._handle);
            }

            Condition& operator=(const Condition& other) {
                _handle = other._handle;
                return *this;
            }

            Condition& operator=(Condition&& other) {
                _handle = move(other._handle);
                return *this;
            }
            
            virtual ~Condition() {
                #ifdef JUTIL_LINUX
                    pthread_cond_destroy(&_handle);
                #endif
            }

            bool wait(Mutex &mutex) {
                #ifdef JUTIL_WINDOWS
                    return SleepConditionVariableCS(_handle, &mutex._handle, INFINITE);
                #elif defined JUTIL_LINUX
                    return pthread_cond_wait(&_handle, &mutex._handle);
                #endif
            }

            void signal() {
                #ifdef JUTIL_WINDOWS
                    WakeConditionVariable(&_handle);
                #elif defined JUTIL_LINUX
                    pthread_cond_signal(&_handle);
                #endif
            }

            private:
                ThreadConditionHandle _handle;
                friend class Mutex;
                friend class Thread;
    };
}

#endif